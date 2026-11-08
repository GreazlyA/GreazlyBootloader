/*******************************************************************************
 * bootloader.c - Greazly bootloader v4
 *
 * 1. LoRa FOTA install (lota_install.h): when the app has left an install
 *    request, rebuild the new image from the stored .lota (FULL or DELTA
 *    against the running app) into the rebuild area, then copy it into the
 *    app slot page by page. Every step is recorded in one-time flash words,
 *    so a reset or power cut at any point resumes where it stopped:
 *      - before "patched" the old app and the .lota are untouched; the
 *        rebuild simply runs again,
 *      - after "patched" the rebuild area holds the verified new image; the
 *        copy continues at the first page not marked as copied.
 *    A request that fails a check is marked failed and the old app starts.
 * 2. Legacy cellular .ota image at 0x28000 (32-byte big-endian header):
 *    copied into the app slot as in v3.
 * 3. Start the app through its vector table (MSP, VTOR, Reset_Handler).
 *
 * The watchdog is started only for an install (same setting as the app);
 * the normal start path does not touch it.
 ******************************************************************************/

#include "main.h"
#include "crc32.h"
#include "em_device.h"
#include "em_msc.h"
#include "lota.h"
#include "lota_install.h"

#define BOOTLOADER_VERSION        4u

/* At LOTA_BOOT_INFO_ADDR (GreazlyBootloader.ld section .boot_info) so the
 * app can tell a v4 bootloader from v3 before it asks for an install. */
__attribute__((used, section(".boot_info")))
const lota_boot_info_t BootInfo = { LOTA_BOOT_INFO_MAGIC, BOOTLOADER_VERSION };

/* Legacy .ota image (cellular FOTA) at the start of the rebuild area */
#define LEGACY_OTA_ADDR           LOTA_REBUILD_ADDR
#define LEGACY_OTA_HEADER_SIZE    32u

/* Same watchdog setting as the app (GreazlyLora main.c mainWatchdogInit):
 * LFRCO 32768 Hz, PERSEL 15 = 8 s, keeps running in EM2/EM3. */
#define BOOT_WDOG_PERSEL          15u

#define BOOT_COPY_ATTEMPTS        3u
#define BOOT_LFRCO_WAIT           2000000u  /* loops, about 1 s at most */

#define RAM_START                 0x20000000UL
#define RAM_END                   0x20008000UL

static lota_install_t const * const Record = (lota_install_t const *)LOTA_INSTALL_ADDR;

/* Word-aligned staging buffer for every flash write */
static uint32_t WriteBuf[LOTA_OUT_CHUNK / 4u];

typedef struct {
    uint32_t addr;          /* next rebuild-area address to program */
    uint32_t end;           /* LOTA_REBUILD_ADDR + to_size */
} rebuild_ctx_t;


uint32_t lota_crc32(const uint8_t *data, uint32_t len)
{
    return crc32BuffCalc((uint8_t *)data, 0, len);
}


static void wdogStart(void)
{
    /* WDOG0 is clocked by the LFRCO (the CMU reset default). This bootloader
     * has no SDK clock setup, so start the oscillator here; without it the
     * watchdog never counts and SYNCBUSY never clears. */
    CMU->CLKEN0_SET = CMU_CLKEN0_WDOG0 | CMU_CLKEN0_LFRCO;
    LFRCO->CTRL_SET = LFRCO_CTRL_FORCEEN;
    for (uint32_t i = 0; i < BOOT_LFRCO_WAIT && !(LFRCO->STATUS & LFRCO_STATUS_RDY); i++)
        ;
    /* A watchdog still running from the app (if the reset kept it) is
     * left as it is and fed. */
    if (!(WDOG0->EN & WDOG_EN_EN) && !(WDOG0->STATUS & WDOG_STATUS_LOCK)) {
        WDOG0->CFG = (BOOT_WDOG_PERSEL << _WDOG_CFG_PERSEL_SHIFT)
                     | WDOG_CFG_EM2RUN | WDOG_CFG_EM3RUN;
        WDOG0->EN = WDOG_EN_EN;
    }
}


/* Never waits: a clear still synchronizing to the LF clock is as good as a
 * new one, and a watchdog without a clock must not stop the install. */
static void wdogFeed(void)
{
    if (!(WDOG0->EN & WDOG_EN_EN) || (WDOG0->SYNCBUSY & WDOG_SYNCBUSY_CMD))
        return;
    WDOG0->CMD = WDOG_CMD_CLEAR;
}


static uint8_t bytesEqual(const uint8_t *a, const uint8_t *b, uint32_t len)
{
    while (len--)
        if (*a++ != *b++)
            return 0;
    return 1;
}


static uint8_t isErased(const uint8_t *p, uint32_t len)
{
    while (len--)
        if (*p++ != 0xFFu)
            return 0;
    return 1;
}


static uint8_t pageErase(uint32_t addr)
{
    wdogFeed();
    MSC_ErasePage((uint32_t *)addr);
    return isErased((const uint8_t *)addr, LOTA_FLASH_PAGE_SIZE);
}


/* Program up to LOTA_OUT_CHUNK bytes from any source (RAM or flash) through
 * the aligned buffer; a tail that is not a multiple of 4 is padded with 0xFF,
 * which is what erased flash holds. Reads the result back. */
static uint8_t flashWrite(uint32_t addr, const uint8_t *data, uint32_t len)
{
    uint8_t *buf = (uint8_t *)WriteBuf;
    uint32_t padded = (len + 3u) & ~3u;
    for (uint32_t i = 0; i < padded; i++)
        buf[i] = (i < len) ? data[i] : 0xFFu;
    MSC_WriteWord((uint32_t *)addr, WriteBuf, padded);
    return bytesEqual((const uint8_t *)addr, buf, padded);
}


static void wordWrite(const uint32_t *word, uint32_t value)
{
    if (*word == LOTA_WORD_ERASED)
        MSC_WriteWord((uint32_t *)word, &value, 4u);
}


/* A mark the bootloader writes (patched, copy_page[n], installed) counts as
 * set as soon as it is not erased: it is programmed only after its step is
 * done and verified, so a power cut that leaves it half programmed still
 * means "done". (Requiring the exact value would leave such a word neither
 * set nor rewritable, and the install would repeat on every boot.) */
static uint8_t markIsSet(uint32_t word)
{
    return word != LOTA_WORD_ERASED;
}


static void stateSet(const uint32_t *word, uint32_t id)
{
    wordWrite(word, Record->to_crc32 ^ id);
}


static void markFailed(uint32_t step, uint32_t code)
{
    wordWrite(&Record->failed, LOTA_FAILED_TAG | (step << 8) | (code & 0xFFu));
}


static uint32_t usedPages(void)
{
    return (Record->to_size + LOTA_FLASH_PAGE_SIZE - 1u) / LOTA_FLASH_PAGE_SIZE;
}


static lota_result_t rebuildWrite(void *ctx, const uint8_t *data, uint32_t len)
{
    rebuild_ctx_t *c = (rebuild_ctx_t *)ctx;
    if (len > c->end - c->addr)
        return LOTA_ERR_WRITE;
    wdogFeed();
    if (!flashWrite(c->addr, data, len))
        return LOTA_ERR_WRITE;
    c->addr += len;
    return LOTA_OK;
}


/* The record must describe the .lota actually in the store. */
static uint8_t recordMatchesStore(void)
{
    if (Record->lota_addr != LOTA_STORE_ADDR ||
        Record->lota_size < LOTA_HEADER_SIZE || Record->lota_size > LOTA_STORE_SIZE)
        return 0;
    if (Record->to_size == 0u || Record->to_size > LOTA_REBUILD_SIZE)
        return 0;
    const lota_header_t *h = (const lota_header_t *)LOTA_STORE_ADDR;
    return h->header_crc32 == Record->lota_header_crc32 &&
           h->to_size == Record->to_size &&
           h->to_crc32 == Record->to_crc32 &&
           h->to_build == Record->to_build;
}


/* Rebuild the new image into the rebuild area. Reads the app slot and the
 * store only, so a reset here leaves everything as it was. */
static void installPatch(void)
{
    lota_result_t r;
    const uint8_t *lota = (const uint8_t *)LOTA_STORE_ADDR;

    if (!recordMatchesStore()) {
        markFailed(LOTA_FAILED_STEP_RECORD, 0u);
        return;
    }
    r = lota_check(lota, Record->lota_size, LOTA_HW_ID,
                   (const uint8_t *)LOTA_APP_ADDR, LOTA_APP_SLOT_SIZE, LOTA_REBUILD_SIZE);
    if (r != LOTA_OK) {
        markFailed(LOTA_FAILED_STEP_CHECK, (uint32_t)r);
        return;
    }

    for (uint32_t n = 0; n < usedPages(); n++)
        if (!pageErase(LOTA_REBUILD_ADDR + n * LOTA_FLASH_PAGE_SIZE))
            return;                         /* flash fault: tried again on the next boot */

    rebuild_ctx_t ctx = { LOTA_REBUILD_ADDR, LOTA_REBUILD_ADDR + Record->to_size };
    r = lota_rebuild(lota, (const uint8_t *)LOTA_APP_ADDR, rebuildWrite, &ctx);
    if (r == LOTA_ERR_WRITE)
        return;                             /* flash fault: tried again on the next boot */
    if (r != LOTA_OK) {
        markFailed(LOTA_FAILED_STEP_REBUILD, (uint32_t)r);
        return;
    }

    wdogFeed();
    if (ctx.addr != ctx.end ||
        lota_crc32((const uint8_t *)LOTA_REBUILD_ADDR, Record->to_size) != Record->to_crc32) {
        markFailed(LOTA_FAILED_STEP_VERIFY, 0u);
        return;
    }
    stateSet(&Record->patched, LOTA_STATE_PATCHED);
}


/* Erase app page n and fill it from src (len bytes, the rest stays 0xFF). */
static uint8_t appPageWrite(uint32_t n, const uint8_t *src, uint32_t len)
{
    uint32_t dst = LOTA_APP_ADDR + n * LOTA_FLASH_PAGE_SIZE;

    for (uint32_t attempt = 0; attempt < BOOT_COPY_ATTEMPTS; attempt++) {
        if (!pageErase(dst))
            continue;
        uint32_t done = 0;
        while (done < len) {
            uint32_t chunk = len - done;
            if (chunk > LOTA_OUT_CHUNK)
                chunk = LOTA_OUT_CHUNK;
            if (!flashWrite(dst + done, src + done, chunk))
                break;
            done += chunk;
        }
        wdogFeed();
        if (done == len && bytesEqual((const uint8_t *)dst, src, len))
            return 1;
    }
    return 0;
}


/* Copy the verified rebuild area into the app slot. Pages already marked are
 * skipped, so a reset continues with the first unmarked page. Pages past the
 * new image are erased, leaving the slot as an SWD-programmed device has it. */
static void installCopy(void)
{
    uint32_t used = usedPages();

    if (!markIsSet(Record->copy_page[0])) {
        /* Nothing copied yet: the old app is still whole, so a bad rebuild
         * area can still fall back to it. */
        if (lota_crc32((const uint8_t *)LOTA_REBUILD_ADDR, Record->to_size) != Record->to_crc32) {
            markFailed(LOTA_FAILED_STEP_VERIFY, 1u);
            return;
        }
    }

    for (uint32_t n = 0; n < LOTA_APP_PAGES; n++) {
        if (markIsSet(Record->copy_page[n]))
            continue;
        uint32_t len = 0;
        if (n < used) {
            len = Record->to_size - n * LOTA_FLASH_PAGE_SIZE;
            if (len > LOTA_FLASH_PAGE_SIZE)
                len = LOTA_FLASH_PAGE_SIZE;
        }
        if (!appPageWrite(n, (const uint8_t *)(LOTA_REBUILD_ADDR + n * LOTA_FLASH_PAGE_SIZE), len))
            NVIC_SystemReset();             /* the rebuild area is intact: try again */
        stateSet(&Record->copy_page[n], LOTA_STATE_COPY_PAGE | n);
    }

    wdogFeed();
    if (lota_crc32((const uint8_t *)LOTA_APP_ADDR, Record->to_size) == Record->to_crc32)
        stateSet(&Record->installed, LOTA_STATE_INSTALLED);
    else
        markFailed(LOTA_FAILED_STEP_INSTALLED, 0u);
}


static uint8_t installPending(void)
{
    if (Record->magic != LOTA_INSTALL_MAGIC ||
        lota_crc32((const uint8_t *)Record, LOTA_INSTALL_RECORD_CRC_LEN) != Record->record_crc32)
        return 0;
    return Record->requested == (Record->to_crc32 ^ LOTA_STATE_REQUESTED) &&
           Record->failed == LOTA_WORD_ERASED &&
           !markIsSet(Record->installed);
}


static void lotaInstall(void)
{
    wdogStart();
    wdogFeed();
    if (!markIsSet(Record->patched))
        installPatch();
    if (markIsSet(Record->patched) && Record->failed == LOTA_WORD_ERASED)
        installCopy();
    /* Installed, failed or a flash fault: start again from a clean reset.
     * The record then asks for a step only if one is still due. */
    NVIC_SystemReset();
}


static uint32_t beWord(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
}


/* v3 behaviour for a cellular .ota: [HeaderLength=0, ImageLength, ImageCrc32,
 * ..., HeaderCrc32] big-endian, image follows. Copied when its CRC is right
 * and the app slot does not already hold it. Restart-safe because the image
 * is erased only after the whole copy. */
static void legacyOtaInstall(void)
{
    const uint8_t *hdr = (const uint8_t *)LEGACY_OTA_ADDR;
    const uint8_t *img = hdr + LEGACY_OTA_HEADER_SIZE;
    uint32_t len = beWord(hdr + 4);
    uint32_t crc = beWord(hdr + 8);

    if (beWord(hdr) != 0u || len == 0u || len > LOTA_APP_SLOT_SIZE)
        return;
    if (lota_crc32(img, len) != crc)
        return;
    if (lota_crc32((const uint8_t *)LOTA_APP_ADDR, len) != crc) {
        for (uint32_t n = 0; n < LOTA_APP_PAGES; n++) {
            uint32_t off = n * LOTA_FLASH_PAGE_SIZE;
            uint32_t part = (off < len) ? len - off : 0u;
            if (part > LOTA_FLASH_PAGE_SIZE)
                part = LOTA_FLASH_PAGE_SIZE;
            if (!appPageWrite(n, img + off, part))
                NVIC_SystemReset();
        }
        if (lota_crc32((const uint8_t *)LOTA_APP_ADDR, len) != crc)
            NVIC_SystemReset();
    }
    MSC_ErasePage((uint32_t *)LEGACY_OTA_ADDR);
    NVIC_SystemReset();
}


/* Start the app through its vector table. With no plausible table the
 * bootloader stays here, so the device can still be reprogrammed over SWD. */
static void appStart(void)
{
    const uint32_t *vectors = (const uint32_t *)LOTA_APP_ADDR;
    uint32_t sp = vectors[0];
    uint32_t pc = vectors[1];

    if (sp <= RAM_START || sp > RAM_END || (sp & 7u) ||
        !(pc & 1u) || pc < LOTA_APP_ADDR || pc >= LOTA_APP_ADDR + LOTA_APP_SLOT_SIZE) {
        for (;;)
            ;
    }

    SCB->VTOR = LOTA_APP_ADDR;
    __set_MSPLIM(0u);
    __DSB();
    __ISB();
    __asm volatile ("msr msp, %0 \n"
                    "bx  %1      \n" : : "r" (sp), "r" (pc) : "memory");
    for (;;)
        ;
}


void Bootloader(void)
{
    if (installPending())
        lotaInstall();                      /* does not return */
    legacyOtaInstall();                     /* returns when there is nothing to copy */
    appStart();
}
