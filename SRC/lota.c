/*******************************************************************************
 * lota.c - .lota LoRa FOTA image: header check and image rebuild
 *
 * Keep in step with fw_lota.py: lzss_unpack() mirrors lota_src_get() and
 * patch_apply() mirrors lota_rebuild(). Every .lota the build makes is
 * rebuilt by that Python mirror before it is written.
 *
 * LOTA_CHECK_ONLY (defined by an includer before #include "lota.c") leaves
 * out lota_rebuild() and its 4.4 KB of RAM: the app only checks files.
 ******************************************************************************/

#include "lota.h"

#ifndef LOTA_CHECK_ONLY

typedef struct {
    const uint8_t *in;
    uint32_t in_size;
    uint32_t in_pos;
    uint32_t out_left;          /* unpacked bytes still to deliver */
    uint8_t  compression;
    uint8_t  wbits;
    uint8_t  lbits;
    uint8_t  bitbuf;
    uint8_t  bitcnt;
    uint16_t ring_pos;
    uint16_t match_dist;
    uint16_t match_left;
    uint8_t  ring[1u << LOTA_LZSS_MAX_WINDOW_BITS];
} lota_src_t;

#define LOTA_RING_MASK  ((1u << LOTA_LZSS_MAX_WINDOW_BITS) - 1u)

static lota_src_t Src;

typedef struct {
    lota_write_fn write;
    void *ctx;
    uint32_t fill;
    uint8_t buf[LOTA_OUT_CHUNK];
} lota_out_t;

static lota_out_t Out;

#endif /* LOTA_CHECK_ONLY */


static lota_header_t header_read(const uint8_t *p)
{
    lota_header_t h;
    uint8_t *d = (uint8_t *)&h;
    for (uint32_t i = 0; i < sizeof(h); i++)
        d[i] = p[i];            /* the stored file need not be aligned */
    return h;
}


lota_result_t lota_check(const uint8_t *lota, uint32_t lota_size, uint32_t hw_id,
                         const uint8_t *old, uint32_t old_slot_size,
                         uint32_t max_new_size)
{
    if (lota_size < LOTA_HEADER_SIZE)
        return LOTA_ERR_SIZE;

    lota_header_t h = header_read(lota);

    if (h.magic != LOTA_MAGIC)
        return LOTA_ERR_MAGIC;
    if (lota_crc32(lota, LOTA_HEADER_SIZE - 4u) != h.header_crc32)
        return LOTA_ERR_HEADER_CRC;
    if (h.format_version != LOTA_FORMAT_VERSION)
        return LOTA_ERR_VERSION;
    if (h.hw_id != hw_id)
        return LOTA_ERR_HW_ID;
    if (h.type != LOTA_TYPE_FULL && h.type != LOTA_TYPE_DELTA)
        return LOTA_ERR_TYPE;

    if (h.compression == LOTA_COMP_LZSS) {
        if (h.lzss_window_bits < 1u || h.lzss_window_bits > LOTA_LZSS_MAX_WINDOW_BITS ||
            h.lzss_length_bits < 1u || h.lzss_length_bits > LOTA_LZSS_MAX_LENGTH_BITS)
            return LOTA_ERR_COMPRESSION;
    } else if (h.compression == LOTA_COMP_NONE) {
        if (h.unpacked_size != h.payload_size)
            return LOTA_ERR_SIZE;
    } else {
        return LOTA_ERR_COMPRESSION;
    }

    if (h.to_size == 0u || h.to_size > max_new_size)
        return LOTA_ERR_SIZE;
    if (h.type == LOTA_TYPE_FULL && h.unpacked_size != h.to_size)
        return LOTA_ERR_SIZE;
    if (h.payload_size > lota_size - LOTA_HEADER_SIZE)
        return LOTA_ERR_SIZE;
    if (lota_crc32(lota + LOTA_HEADER_SIZE, h.payload_size) != h.payload_crc32)
        return LOTA_ERR_PAYLOAD_CRC;

    if (h.type == LOTA_TYPE_DELTA) {
        if (h.from_size == 0u || h.from_size > old_slot_size)
            return LOTA_ERR_BASE;
        if (lota_crc32(old, h.from_size) != h.from_crc32)
            return LOTA_ERR_BASE;
    }
    return LOTA_OK;
}


#ifndef LOTA_CHECK_ONLY

static lota_result_t src_bits(uint8_t count, uint32_t *value)
{
    uint32_t v = 0;
    while (count--) {
        if (Src.bitcnt == 0u) {
            if (Src.in_pos >= Src.in_size)
                return LOTA_ERR_TRUNCATED;
            Src.bitbuf = Src.in[Src.in_pos++];
            Src.bitcnt = 8u;
        }
        Src.bitcnt--;
        v = (v << 1) | ((uint32_t)(Src.bitbuf >> Src.bitcnt) & 1u);
    }
    *value = v;
    return LOTA_OK;
}


/* Next unpacked payload byte. */
static lota_result_t lota_src_get(uint8_t *byte)
{
    if (Src.out_left == 0u)
        return LOTA_ERR_TRUNCATED;
    Src.out_left--;

    if (Src.compression == LOTA_COMP_NONE) {
        if (Src.in_pos >= Src.in_size)
            return LOTA_ERR_TRUNCATED;
        *byte = Src.in[Src.in_pos++];
        return LOTA_OK;
    }

    uint32_t v;
    lota_result_t r;
    if (Src.match_left == 0u) {
        if ((r = src_bits(1u, &v)) != LOTA_OK)
            return r;
        if (v) {
            if ((r = src_bits(8u, &v)) != LOTA_OK)
                return r;
            Src.ring[Src.ring_pos] = (uint8_t)v;
            Src.ring_pos = (uint16_t)((Src.ring_pos + 1u) & LOTA_RING_MASK);
            *byte = (uint8_t)v;
            return LOTA_OK;
        }
        if ((r = src_bits(Src.wbits, &v)) != LOTA_OK)
            return r;
        Src.match_dist = (uint16_t)(v + 1u);
        if ((r = src_bits(Src.lbits, &v)) != LOTA_OK)
            return r;
        Src.match_left = (uint16_t)(v + LOTA_LZSS_MIN_MATCH);
    }
    uint8_t b = Src.ring[(Src.ring_pos - Src.match_dist) & LOTA_RING_MASK];
    Src.ring[Src.ring_pos] = b;
    Src.ring_pos = (uint16_t)((Src.ring_pos + 1u) & LOTA_RING_MASK);
    Src.match_left--;
    *byte = b;
    return LOTA_OK;
}


static lota_result_t get_uvarint(uint32_t *value)
{
    uint32_t v = 0;
    uint8_t shift = 0;
    uint8_t b;
    lota_result_t r;
    for (;;) {
        if ((r = lota_src_get(&b)) != LOTA_OK)
            return r;
        if (shift > 21u)                    /* at most 4 bytes, value < 2^28 */
            return LOTA_ERR_VARINT;
        v |= (uint32_t)(b & 0x7Fu) << shift;
        if (!(b & 0x80u))
            break;
        shift = (uint8_t)(shift + 7u);
    }
    *value = v;
    return LOTA_OK;
}


static lota_result_t out_byte(uint8_t b)
{
    Out.buf[Out.fill++] = b;
    if (Out.fill == LOTA_OUT_CHUNK) {
        Out.fill = 0;
        return Out.write(Out.ctx, Out.buf, LOTA_OUT_CHUNK);
    }
    return LOTA_OK;
}


static lota_result_t out_flush(void)
{
    uint32_t n = Out.fill;
    Out.fill = 0;
    return n ? Out.write(Out.ctx, Out.buf, n) : LOTA_OK;
}


lota_result_t lota_rebuild(const uint8_t *lota, const uint8_t *old,
                           lota_write_fn write, void *ctx)
{
    lota_header_t h = header_read(lota);
    lota_result_t r;
    uint8_t b;

    Src.in = lota + LOTA_HEADER_SIZE;
    Src.in_size = h.payload_size;
    Src.in_pos = 0;
    Src.out_left = h.unpacked_size;
    Src.compression = h.compression;
    Src.wbits = h.lzss_window_bits;
    Src.lbits = h.lzss_length_bits;
    Src.bitbuf = 0;
    Src.bitcnt = 0;
    Src.ring_pos = 0;
    Src.match_dist = 0;
    Src.match_left = 0;
    for (uint32_t i = 0; i < sizeof(Src.ring); i++)
        Src.ring[i] = 0;

    Out.write = write;
    Out.ctx = ctx;
    Out.fill = 0;

    if (h.type == LOTA_TYPE_FULL) {
        for (uint32_t i = 0; i < h.to_size; i++) {
            if ((r = lota_src_get(&b)) != LOTA_OK)
                return r;
            if ((r = out_byte(b)) != LOTA_OK)
                return LOTA_ERR_WRITE;
        }
        return out_flush() == LOTA_OK ? LOTA_OK : LOTA_ERR_WRITE;
    }

    /* DELTA: records of (diff_len, extra_len, seek) until to_size is made */
    uint32_t new_pos = 0;
    int32_t old_pos = 0;
    while (new_pos < h.to_size) {
        uint32_t diff_len, extra_len, z;
        if ((r = get_uvarint(&diff_len)) != LOTA_OK) return r;
        if ((r = get_uvarint(&extra_len)) != LOTA_OK) return r;
        if ((r = get_uvarint(&z)) != LOTA_OK) return r;
        int32_t seek = (int32_t)(z >> 1) ^ -(int32_t)(z & 1u);

        if (diff_len > h.to_size - new_pos ||
            extra_len > h.to_size - new_pos - diff_len)
            return LOTA_ERR_RANGE;
        if (old_pos < 0 || (uint32_t)old_pos > h.from_size ||
            diff_len > h.from_size - (uint32_t)old_pos)
            return LOTA_ERR_RANGE;

        for (uint32_t i = 0; i < diff_len; i++) {
            if ((r = lota_src_get(&b)) != LOTA_OK)
                return r;
            if (out_byte((uint8_t)(old[(uint32_t)old_pos + i] + b)) != LOTA_OK)
                return LOTA_ERR_WRITE;
        }
        old_pos += (int32_t)diff_len;
        for (uint32_t i = 0; i < extra_len; i++) {
            if ((r = lota_src_get(&b)) != LOTA_OK)
                return r;
            if (out_byte(b) != LOTA_OK)
                return LOTA_ERR_WRITE;
        }
        old_pos += seek;
        new_pos += diff_len + extra_len;
    }
    if (Src.out_left != 0u)
        return LOTA_ERR_TRAILING;
    return out_flush() == LOTA_OK ? LOTA_OK : LOTA_ERR_WRITE;
}

#endif /* LOTA_CHECK_ONLY */
