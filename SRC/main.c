/*******************************************************************************
 * main.c - Greazly bootloader entry
 *
 * Runs on the reset clock (FSRCO, 20 MHz) without the Simplicity SDK system
 * init: the bootloader needs only the flash controller, and leaving out
 * sl_system_init() (HFXO, DCDC, clock trees) keeps it far below the 0x1FA8
 * key block. The app does its own full init when it starts.
 ******************************************************************************/

#include "em_chip.h"
#include "em_msc.h"
#include "bootloader.h"

int main(void)
{
  CHIP_Init();      // device errata fixes
  MSC_Init();       // flash controller clock, unlock, write enable

  Bootloader();     // does not return

  for (;;)
    ;
}
