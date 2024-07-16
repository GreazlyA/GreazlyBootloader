/***************************************************************************//**
 * @file main.c
 * @brief main() function.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "sl_component_catalog.h"
#include "sl_system_init.h"
#include "app.h"
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "sl_system_kernel.h"
#else // SL_CATALOG_KERNEL_PRESENT
#include "sl_system_process_action.h"
#endif // SL_CATALOG_KERNEL_PRESENT

#include  "types.h"
#include  "pin_config.h"
#include  "main.h"
#include  "FLASH/flash.h"
#include  "main.h"
#include "em_gpio.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "Bootloader.h"



// P R O T O T Y P E S


// G L O B A L S
uint32_t Delay;
uint32_t OperationFlag;

int main(void)
{

  // Initialize Silicon Labs device, system, service(s) and CellProtocol stack(s).
  // Note that if the kernel is present, processing task(s) will be created by
  // this call.
  sl_system_init();


  // LatchIO is enabled by the siliconlabs initialization sequence
  MSC_Init();


//  uartsDebugStringWrite(">>>>>>>>>>>>>>>>>>>>>>>>> BOOTLOADER >>>>>>>\n\r\n\r\n\r");

  // Start the application

  Bootloader();


}







