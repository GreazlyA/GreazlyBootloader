/******************************************************************************
 * File name	:  	types.h
 *
 *
 ********************************************************************************/

#ifndef TYPES_H_
#define TYPES_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>

#include "pin_config.h"

#include "em_device.h"
#include "em_chip.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_msc.h"
#include "string.h"

#include "bgm22_msc.h"





typedef enum  { RETURNCODE_OK=0, RETURNCODE_ERROR, RETURNCODE_UNSUPPORTED, RETURNCODE_PROTOCOLDLL_IDLE, RETURNCODE_PROTOCOLDLL_BUSY} ReturnCode_T;




#ifndef TRUE
#define TRUE         true
#endif
#ifndef FALSE
#define FALSE        false
#endif

#ifndef NULL
#define NULL 		(	(void *)0	)
#endif

#ifndef IN
#define IN 			const
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef __INLINE
#define __INLINE			inline
#endif

#ifndef __STATIC_INLINE
#define __STATIC_INLINE		static inline
#endif

#define PA_HANDLE						uint32_t	// Peripheral Access Handle







typedef enum  {TASK_DISABLED, TASK_ENABLED} TaskFsm_T;
/* End Of Global Variables  */


#endif /* TYPES_H_ */
