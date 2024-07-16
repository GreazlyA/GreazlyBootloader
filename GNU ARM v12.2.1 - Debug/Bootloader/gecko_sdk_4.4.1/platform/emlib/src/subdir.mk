################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_cmu.c \
../Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_core.c \
../Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_emu.c \
../Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_gpio.c \
../Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_msc.c \
../Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_system.c \
../Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_usart.c 

OBJS += \
./Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_cmu.o \
./Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_core.o \
./Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_emu.o \
./Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_gpio.o \
./Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_msc.o \
./Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_system.o \
./Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_usart.o 

C_DEPS += \
./Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_cmu.d \
./Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_core.d \
./Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_emu.d \
./Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_gpio.d \
./Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_msc.d \
./Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_system.d \
./Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_usart.d 


# Each subdirectory must supply rules for building sources it contributes
Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_cmu.o: ../Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_cmu.c Bootloader/gecko_sdk_4.4.1/platform/emlib/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DBGM220SC22HNA=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\FLASH" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\MEM" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\config" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\autogen" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\Device\SiliconLabs\BGM22\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\CMSIS\Core\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\device_init\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\emlib\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\toolchain\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\system\inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_cmu.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_core.o: ../Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_core.c Bootloader/gecko_sdk_4.4.1/platform/emlib/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DBGM220SC22HNA=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\FLASH" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\MEM" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\config" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\autogen" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\Device\SiliconLabs\BGM22\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\CMSIS\Core\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\device_init\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\emlib\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\toolchain\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\system\inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_core.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_emu.o: ../Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_emu.c Bootloader/gecko_sdk_4.4.1/platform/emlib/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DBGM220SC22HNA=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\FLASH" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\MEM" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\config" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\autogen" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\Device\SiliconLabs\BGM22\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\CMSIS\Core\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\device_init\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\emlib\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\toolchain\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\system\inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_emu.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_gpio.o: ../Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_gpio.c Bootloader/gecko_sdk_4.4.1/platform/emlib/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DBGM220SC22HNA=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\FLASH" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\MEM" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\config" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\autogen" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\Device\SiliconLabs\BGM22\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\CMSIS\Core\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\device_init\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\emlib\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\toolchain\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\system\inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_gpio.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_msc.o: ../Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_msc.c Bootloader/gecko_sdk_4.4.1/platform/emlib/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DBGM220SC22HNA=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\FLASH" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\MEM" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\config" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\autogen" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\Device\SiliconLabs\BGM22\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\CMSIS\Core\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\device_init\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\emlib\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\toolchain\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\system\inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_msc.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_system.o: ../Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_system.c Bootloader/gecko_sdk_4.4.1/platform/emlib/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DBGM220SC22HNA=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\FLASH" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\MEM" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\config" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\autogen" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\Device\SiliconLabs\BGM22\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\CMSIS\Core\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\device_init\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\emlib\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\toolchain\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\system\inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_system.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_usart.o: ../Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_usart.c Bootloader/gecko_sdk_4.4.1/platform/emlib/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DBGM220SC22HNA=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\FLASH" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\MEM" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\config" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\autogen" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\Device\SiliconLabs\BGM22\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\CMSIS\Core\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\device_init\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\emlib\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\toolchain\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\system\inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"Bootloader/gecko_sdk_4.4.1/platform/emlib/src/em_usart.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


