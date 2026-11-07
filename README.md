# Greazly bootloader (v4)

Bootloader for the GreazlyLora BGM22 board. It sits at 0x0000–0x1FA7, below the boot info at 0x1FA0 and the per-device key block at 0x1FA8–0x1FFF, and starts the app at 0x2000.

On each reset it does one of three things:

1. **LoRa FOTA install.** If the app has left an install request (record at 0x4E000), it rebuilds the new image from the `.lota` file at 0x40000 into 0x28000. The file holds either a full image or a delta against the running app. It then copies the new image into the app slot one page at a time. Each step is recorded in one-time flash words, so a reset or power cut resumes where it stopped. If a check fails, it marks the request failed and starts the old app.
2. **Legacy cellular `.ota`** at 0x28000. It copies the image as v3 did.
3. **App start.** It jumps through the app's vector table (MSP, VTOR, Reset_Handler).

| File | Contents |
|---|---|
| `SRC/bootloader.c` | Install, legacy copy and app start |
| `SRC/main.c` | `CHIP_Init`, `MSC_Init`, then `Bootloader()`. There's no `sl_system_init`, so it runs on the reset clock. |
| `SRC/lota.c`, `lota.h`, `lota_install.h` | Copies from `GreazlyLora/FW/Lota`. Change them there, then copy them here. |
| `GreazlyBootloader.ld` | Linker script. FLASH ends at 0x1FA8. RAM is 0x20005000–0x20007FFF, above the data the app keeps across a reset. It has size asserts and the `.boot_info` section at 0x1FA0. The project links this file, not `autogen/linkerfile.ld`, which Studio regenerates. |

Size: 4 052 B of code and 12 B of data, so flash ends at 0x0FD8. The build fails if the bootloader reaches 0x1FA0.

**Test:** `test/emu_test.py` runs the built image in a Cortex-M33 emulator with a model of the flash controller. It covers FULL and DELTA installs, rejected files, the legacy `.ota`, an erased app, and a power cut at every record write, every erase and 120 random points. Last run: all passed. The proposal's §7.6 has the command and the results.

The bootloader cannot update itself. Install it over SWD, together with the app and the keys (`GreazlyLoraProduction/hex_file_generator.py`).

The full design is in `LoraTestDESKTOP-B9NF4MV/docs/lora_fota_proposal.md` §7.
