################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../gecko_sdk_4.4.1/platform/common/toolchain/src/sl_memory.c 

OBJS += \
./gecko_sdk_4.4.1/platform/common/toolchain/src/sl_memory.o 

C_DEPS += \
./gecko_sdk_4.4.1/platform/common/toolchain/src/sl_memory.d 


# Each subdirectory must supply rules for building sources it contributes
gecko_sdk_4.4.1/platform/common/toolchain/src/sl_memory.o: ../gecko_sdk_4.4.1/platform/common/toolchain/src/sl_memory.c gecko_sdk_4.4.1/platform/common/toolchain/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DBGM220SC22HNA=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"D:\Users\nissi\Documents\Projects\Greazly\Bootloader\SRC" -I"D:\Users\nissi\Documents\Projects\Greazly\Bootloader\SRC\FLASH" -I"D:\Users\nissi\Documents\Projects\Greazly\Bootloader\SRC\MEM" -I"D:\Users\nissi\Documents\Projects\Greazly\Bootloader\config" -I"D:\Users\nissi\Documents\Projects\Greazly\Bootloader\autogen" -I"D:\Users\nissi\Documents\Projects\Greazly\Bootloader" -I"D:\Users\nissi\Documents\Projects\Greazly\Bootloader\gecko_sdk_4.4.1\platform\Device\SiliconLabs\BGM22\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\Bootloader\gecko_sdk_4.4.1\platform\CMSIS\Core\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\Bootloader\gecko_sdk_4.4.1\platform\service\device_init\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\Bootloader\gecko_sdk_4.4.1\platform\emlib\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\Bootloader\gecko_sdk_4.4.1\platform\common\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\Bootloader\gecko_sdk_4.4.1\platform\common\toolchain\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\Bootloader\gecko_sdk_4.4.1\platform\service\system\inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_4.4.1/platform/common/toolchain/src/sl_memory.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


