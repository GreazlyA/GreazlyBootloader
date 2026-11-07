/*******************************************************************************
 * lota_install.h - flash map and install record shared by the application
 * and bootloader v4 (GreazlyBootloader SRC/lota_install.h is a copy: keep the
 * two files identical).
 *
 * Flow:
 *   1. The app receives a .lota into the store, checks it (lota_check() and,
 *      when signed, the signature) and erases the record page.
 *   2. The app programs the record fields, then record_crc32, then
 *      requested = to_crc32 ^ LOTA_STATE_REQUESTED, and resets.
 *   3. The bootloader rebuilds the image into the rebuild area and marks
 *      patched, copies it into the app slot page by page (copy_page[n]) and
 *      marks installed, or marks failed and starts the old app.
 *
 * Every state word is programmed once from 0xFFFFFFFF and never erased by the
 * bootloader, so a reset at any point resumes at the first unmarked step.
 *
 * requested must hold exactly to_crc32 ^ LOTA_STATE_REQUESTED. The marks the
 * bootloader writes (patched, copy_page[n], installed, failed) count as set
 * as soon as they are not 0xFFFFFFFF: each is written only after its step is
 * done and verified, so one half programmed by a power cut still means done.
 * The app therefore must erase the record page and check that it reads all
 * 0xFF before it writes a new record.
 ******************************************************************************/

#ifndef LOTA_INSTALL_H
#define LOTA_INSTALL_H

#include <stdint.h>

#define LOTA_HW_ID                  1u             /* fw_lota.py DEFAULT_HW_ID */

#define LOTA_FLASH_PAGE_SIZE        0x2000u        /* 8 KB */

/* Bootloader identification, last 8 bytes before the device key block
 * (0x1FA8). Bootloader v3 has 0xFF here, so it cannot install a .lota:
 * the app must not ask for an install unless magic matches and
 * version >= LOTA_BOOTLOADER_MIN_VERSION. */
#define LOTA_BOOT_INFO_ADDR         0x00001FA0u
#define LOTA_BOOT_INFO_MAGIC        0x564C4247UL   /* "GBLV" in memory order */
#define LOTA_BOOTLOADER_MIN_VERSION 4u

typedef struct {
    uint32_t magic;               /* LOTA_BOOT_INFO_MAGIC */
    uint32_t version;             /* bootloader version   */
} lota_boot_info_t;

#define LOTA_APP_ADDR               0x00002000u    /* app slot: 0x2000-0x27FFF */
#define LOTA_APP_SLOT_SIZE          0x00026000u
#define LOTA_APP_PAGES              (LOTA_APP_SLOT_SIZE / LOTA_FLASH_PAGE_SIZE)  /* 19 */

#define LOTA_REBUILD_ADDR           0x00028000u    /* rebuilt new image: 0x28000-0x3FFFF */
#define LOTA_REBUILD_SIZE           0x00018000u    /* 96 KB: the largest image LoRa FOTA can install */

#define LOTA_STORE_ADDR             0x00040000u    /* received .lota: 0x40000-0x4DFFF */
#define LOTA_STORE_SIZE             0x0000E000u    /* 56 KB */

#define LOTA_INSTALL_ADDR           0x0004E000u    /* install record page: 0x4E000-0x4FFFF */

#define LOTA_INSTALL_MAGIC          0x534E494CUL   /* "LINS" in memory order */

/* State word values are to_crc32 ^ id, so a word left over from another
 * image or half programmed by a power cut never reads as set. */
#define LOTA_STATE_REQUESTED        0x52455155UL   /* "UQER" */
#define LOTA_STATE_PATCHED          0x50415443UL
#define LOTA_STATE_INSTALLED        0x494E5354UL
#define LOTA_STATE_COPY_PAGE        0x43500000UL   /* | page index */

/* failed = LOTA_FAILED_TAG | (step << 8) | lota_result_t or other reason */
#define LOTA_FAILED_TAG             0xFA110000UL
#define LOTA_FAILED_STEP_RECORD     1u             /* record fields do not match the stored .lota */
#define LOTA_FAILED_STEP_CHECK      2u             /* lota_check() rejected the file */
#define LOTA_FAILED_STEP_REBUILD    3u             /* lota_rebuild() failed */
#define LOTA_FAILED_STEP_VERIFY     4u             /* rebuild area CRC is not to_crc32 */
#define LOTA_FAILED_STEP_INSTALLED  5u             /* app slot CRC after the copy is not to_crc32 */

#define LOTA_WORD_ERASED            0xFFFFFFFFUL

typedef struct {
    /* written by the app, in this order */
    uint32_t magic;               /* LOTA_INSTALL_MAGIC                          */
    uint32_t lota_addr;           /* LOTA_STORE_ADDR                             */
    uint32_t lota_size;           /* bytes of the stored .lota                   */
    uint32_t to_build;            /* from the .lota header                       */
    uint32_t to_size;             /* from the .lota header                       */
    uint32_t to_crc32;            /* from the .lota header                       */
    uint32_t lota_header_crc32;   /* header_crc32 of the stored .lota            */
    uint32_t record_crc32;        /* CRC32 of the 7 words above                  */
    uint32_t requested;           /* to_crc32 ^ LOTA_STATE_REQUESTED             */
    /* written by the bootloader */
    uint32_t patched;             /* to_crc32 ^ LOTA_STATE_PATCHED               */
    uint32_t failed;              /* LOTA_FAILED_TAG | step << 8 | code          */
    uint32_t installed;           /* to_crc32 ^ LOTA_STATE_INSTALLED             */
    uint32_t copy_page[LOTA_APP_PAGES];  /* to_crc32 ^ (LOTA_STATE_COPY_PAGE | n) */
} lota_install_t;

#define LOTA_INSTALL_RECORD_CRC_LEN 28u            /* magic .. lota_header_crc32 */

_Static_assert(sizeof(lota_install_t) == (12u + LOTA_APP_PAGES) * 4u, "lota_install_t layout");
_Static_assert(LOTA_REBUILD_ADDR == LOTA_APP_ADDR + LOTA_APP_SLOT_SIZE, "rebuild area follows the app slot");
_Static_assert(LOTA_STORE_ADDR == LOTA_REBUILD_ADDR + LOTA_REBUILD_SIZE, "store follows the rebuild area");
_Static_assert(LOTA_INSTALL_ADDR == LOTA_STORE_ADDR + LOTA_STORE_SIZE, "record page follows the store");

#endif /* LOTA_INSTALL_H */
