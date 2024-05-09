################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include gecko_sdk_4.4.1/platform/service/system/src/subdir.mk
-include gecko_sdk_4.4.1/platform/service/device_init/src/subdir.mk
-include gecko_sdk_4.4.1/platform/emlib/src/subdir.mk
-include gecko_sdk_4.4.1/platform/common/toolchain/src/subdir.mk
-include gecko_sdk_4.4.1/platform/common/src/subdir.mk
-include gecko_sdk_4.4.1/platform/Device/SiliconLabs/BGM22/Source/subdir.mk
-include autogen/subdir.mk
-include SRC/MEM/subdir.mk
-include SRC/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := GreazlyBootloaderProject
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: main-build

# Main-build Target
main-build: GreazlyBootloaderProject.axf

# Tool invocations
GreazlyBootloaderProject.axf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -T "D:\Users\nissi\Documents\Projects\Greazly\Bootloader\autogen\linkerfile.ld" -Wl,--no-warn-rwx-segments -Xlinker --gc-sections -Xlinker -Map="GreazlyBootloaderProject.map" -mfpu=fpv5-sp-d16 -mfloat-abi=hard --specs=nano.specs -o GreazlyBootloaderProject.axf -Wl,--start-group "./SRC/MEM/mem.o" "./SRC/app.o" "./SRC/bootloader.o" "./SRC/crc32.o" "./SRC/main.o" "./autogen/sl_device_init_clocks.o" "./autogen/sl_event_handler.o" "./gecko_sdk_4.4.1/platform/Device/SiliconLabs/BGM22/Source/startup_bgm22.o" "./gecko_sdk_4.4.1/platform/Device/SiliconLabs/BGM22/Source/system_bgm22.o" "./gecko_sdk_4.4.1/platform/common/src/sl_assert.o" "./gecko_sdk_4.4.1/platform/common/src/sl_syscalls.o" "./gecko_sdk_4.4.1/platform/common/toolchain/src/sl_memory.o" "./gecko_sdk_4.4.1/platform/emlib/src/em_cmu.o" "./gecko_sdk_4.4.1/platform/emlib/src/em_core.o" "./gecko_sdk_4.4.1/platform/emlib/src/em_emu.o" "./gecko_sdk_4.4.1/platform/emlib/src/em_gpio.o" "./gecko_sdk_4.4.1/platform/emlib/src/em_msc.o" "./gecko_sdk_4.4.1/platform/emlib/src/em_system.o" "./gecko_sdk_4.4.1/platform/emlib/src/em_usart.o" "./gecko_sdk_4.4.1/platform/service/device_init/src/sl_device_init_dcdc_s2.o" "./gecko_sdk_4.4.1/platform/service/device_init/src/sl_device_init_emu_s2.o" "./gecko_sdk_4.4.1/platform/service/device_init/src/sl_device_init_hfxo_s2.o" "./gecko_sdk_4.4.1/platform/service/device_init/src/sl_device_init_nvic.o" "./gecko_sdk_4.4.1/platform/service/system/src/sl_system_init.o" "./gecko_sdk_4.4.1/platform/service/system/src/sl_system_process_action.o" -lgcc -lc -lm -lnosys -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building out file: GreazlyBootloaderProject.out'
	arm-none-eabi-objcopy "GreazlyBootloaderProject.axf" "GreazlyBootloaderProject.out"
	@echo ' '

	@echo 'Building hex file: GreazlyBootloaderProject.hex'
	arm-none-eabi-objcopy -O ihex "GreazlyBootloaderProject.axf" "GreazlyBootloaderProject.hex"
	@echo ' '

	@echo 'Building bin file: GreazlyBootloaderProject.bin'
	arm-none-eabi-objcopy -O binary "GreazlyBootloaderProject.axf" "GreazlyBootloaderProject.bin"
	@echo ' '

	@echo 'Building s37 file: GreazlyBootloaderProject.s37'
	arm-none-eabi-objcopy -O srec "GreazlyBootloaderProject.axf" "GreazlyBootloaderProject.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "GreazlyBootloaderProject.axf" -A
	@echo ' '

# Other Targets
clean:
	-@$(foreach FILE, $(EXECUTABLES), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(OBJS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(C_DEPS), $(shell $(RM) $(FILE)) )
	-$(RM) GreazlyBootloaderProject.axf
	-@echo ' '

.PHONY: all clean dependents main-build

-include ../makefile.targets
