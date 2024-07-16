################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Bootloader/SRC/app.c \
../Bootloader/SRC/bootloader.c \
../Bootloader/SRC/crc32.c \
../Bootloader/SRC/main.c 

OBJS += \
./Bootloader/SRC/app.o \
./Bootloader/SRC/bootloader.o \
./Bootloader/SRC/crc32.o \
./Bootloader/SRC/main.o 

C_DEPS += \
./Bootloader/SRC/app.d \
./Bootloader/SRC/bootloader.d \
./Bootloader/SRC/crc32.d \
./Bootloader/SRC/main.d 


# Each subdirectory must supply rules for building sources it contributes
Bootloader/SRC/app.o: ../Bootloader/SRC/app.c Bootloader/SRC/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DBGM220SC22HNA=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\FLASH" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\MEM" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\config" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\autogen" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\Device\SiliconLabs\BGM22\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\CMSIS\Core\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\device_init\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\emlib\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\toolchain\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\system\inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"Bootloader/SRC/app.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Bootloader/SRC/bootloader.o: ../Bootloader/SRC/bootloader.c Bootloader/SRC/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DBGM220SC22HNA=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\FLASH" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\MEM" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\config" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\autogen" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\Device\SiliconLabs\BGM22\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\CMSIS\Core\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\device_init\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\emlib\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\toolchain\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\system\inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"Bootloader/SRC/bootloader.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Bootloader/SRC/crc32.o: ../Bootloader/SRC/crc32.c Bootloader/SRC/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DBGM220SC22HNA=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\FLASH" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\MEM" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\config" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\autogen" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\Device\SiliconLabs\BGM22\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\CMSIS\Core\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\device_init\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\emlib\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\toolchain\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\system\inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"Bootloader/SRC/crc32.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Bootloader/SRC/main.o: ../Bootloader/SRC/main.c Bootloader/SRC/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DBGM220SC22HNA=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\FLASH" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\SRC\MEM" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\config" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\autogen" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\Device\SiliconLabs\BGM22\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\CMSIS\Core\Include" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\device_init\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\emlib\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\common\toolchain\inc" -I"D:\Users\nissi\Documents\Projects\Greazly\GreazlyBootloader\gecko_sdk_4.4.1\platform\service\system\inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"Bootloader/SRC/main.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


