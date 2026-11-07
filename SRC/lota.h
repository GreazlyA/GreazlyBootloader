/*******************************************************************************
 * lota.h - .lota LoRa FOTA image: header check and image rebuild
 *
 * Shared by the bootloader (rebuilds the image) and the application (checks a
 * received file before it asks for the install). The file format is described
 * in fw_lota.py, which also holds a Python mirror of this code that the build
 * self-test runs on every generated file.
 *
 * No dynamic memory, no libc beyond <stdint.h>. RAM: one static lota_src_t
 * (about 4.1 KB, the LZSS window) plus a 256-byte output buffer.
 *
 * Integrator provides:
 *   uint32_t lota_crc32(const uint8_t *data, uint32_t len);
 *       zlib CRC32 (init 0xFFFFFFFF, reflected 0xEDB88320, final xor) -
 *       the same value as crc32BuffCalc(data, 0, len) in the firmware.
 ******************************************************************************/

#ifndef LOTA_H
#define LOTA_H

#include <stdint.h>

#define LOTA_MAGIC                  0x41544F4CUL   /* "LOTA" in file order */
#define LOTA_FORMAT_VERSION         1u
#define LOTA_HEADER_SIZE            64u

#define LOTA_TYPE_FULL              0u
#define LOTA_TYPE_DELTA             1u

#define LOTA_COMP_NONE              0u
#define LOTA_COMP_LZSS              1u

#define LOTA_FLAG_SIGNED            0x01u          /* 64-byte signature after payload */

#define LOTA_LZSS_MIN_MATCH         3u
#define LOTA_LZSS_MAX_WINDOW_BITS   12u            /* ring buffer = 4 KB */
#define LOTA_LZSS_MAX_LENGTH_BITS   12u

#define LOTA_OUT_CHUNK              256u           /* bytes per write callback */

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint8_t  format_version;
    uint8_t  type;
    uint8_t  compression;
    uint8_t  flags;
    uint32_t hw_id;
    uint32_t from_build;
    uint32_t from_size;
    uint32_t from_crc32;
    uint32_t to_build;
    uint32_t to_size;
    uint32_t to_crc32;
    uint32_t payload_size;
    uint32_t payload_crc32;
    uint8_t  lzss_window_bits;
    uint8_t  lzss_length_bits;
    uint16_t reserved1;
    uint32_t unpacked_size;
    uint8_t  reserved2[8];
    uint32_t header_crc32;
} lota_header_t;

_Static_assert(sizeof(lota_header_t) == LOTA_HEADER_SIZE, "lota header must be 64 bytes");

typedef enum {
    LOTA_OK = 0,
    LOTA_ERR_MAGIC,          /* not a .lota file                                */
    LOTA_ERR_HEADER_CRC,     /* header damaged                                  */
    LOTA_ERR_VERSION,        /* format version not supported                    */
    LOTA_ERR_HW_ID,          /* file is for another product                     */
    LOTA_ERR_TYPE,           /* unknown type                                    */
    LOTA_ERR_COMPRESSION,    /* unknown compression or parameters               */
    LOTA_ERR_SIZE,           /* sizes inconsistent or too large for the slots   */
    LOTA_ERR_PAYLOAD_CRC,    /* payload damaged                                 */
    LOTA_ERR_BASE,           /* DELTA: running image is not the base it needs   */
    LOTA_ERR_TRUNCATED,      /* payload ended early                             */
    LOTA_ERR_VARINT,         /* malformed patch record                          */
    LOTA_ERR_RANGE,          /* patch record outside the old or new image       */
    LOTA_ERR_TRAILING,       /* payload longer than its records                 */
    LOTA_ERR_WRITE,          /* write callback failed                           */
} lota_result_t;

/* Write callback: len is a multiple of 4 except possibly on the last call. */
typedef lota_result_t (*lota_write_fn)(void *ctx, const uint8_t *data, uint32_t len);

extern uint32_t lota_crc32(const uint8_t *data, uint32_t len);

/*
 * Full check of a stored .lota before anything is erased:
 *   header magic, CRC, version, hw_id, type, compression parameters, sizes,
 *   payload inside lota_size, payload CRC, and for a DELTA that the running
 *   image (old, at most old_slot_size bytes) is the exact base:
 *   CRC32(old[0..from_size)) == from_crc32.
 * max_new_size is the size of the destination area.
 */
lota_result_t lota_check(const uint8_t *lota, uint32_t lota_size, uint32_t hw_id,
                         const uint8_t *old, uint32_t old_slot_size,
                         uint32_t max_new_size);

/*
 * Rebuild the new image and hand it to write() in order, LOTA_OUT_CHUNK bytes
 * at a time. Call only after lota_check() returned LOTA_OK. old is only read.
 * The caller verifies to_crc32 over what was written before using it.
 */
lota_result_t lota_rebuild(const uint8_t *lota, const uint8_t *old,
                           lota_write_fn write, void *ctx);

#endif /* LOTA_H */
