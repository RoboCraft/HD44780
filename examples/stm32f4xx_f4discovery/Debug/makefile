################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include StdPeripheralDriver/src/subdir.mk
-include CMSIS/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
stm32f4d_lcd_test.hex \

SECONDARY_LIST += \
stm32f4d_lcd_test.lst \

SECONDARY_SIZE += \
stm32f4d_lcd_test.siz \


# All Target
all: stm32f4d_lcd_test.elf secondary-outputs

# Tool invocations
stm32f4d_lcd_test.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Sourcery Linux GCC C Linker'
	arm-none-eabi-gcc -T"/home/burjui/devel/stm32/stm32f4d_lcd_test/stm32f407vg.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,stm32f4d_lcd_test.map -mcpu=cortex-m4 -mthumb -g3 -gdwarf-2 -o"stm32f4d_lcd_test.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

stm32f4d_lcd_test.hex: stm32f4d_lcd_test.elf
	@echo 'Invoking: ARM Sourcery Linux GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex stm32f4d_lcd_test.elf "stm32f4d_lcd_test.hex"
	@echo 'Finished building: $@'
	@echo ' '

stm32f4d_lcd_test.lst: stm32f4d_lcd_test.elf
	@echo 'Invoking: ARM Sourcery Linux GNU Create Listing'
	arm-none-eabi-objdump -h -S stm32f4d_lcd_test.elf >"stm32f4d_lcd_test.lst"
	@echo 'Finished building: $@'
	@echo ' '

stm32f4d_lcd_test.siz: stm32f4d_lcd_test.elf
	@echo 'Invoking: ARM Sourcery Linux GNU Print Size'
	arm-none-eabi-size  --format=berkeley stm32f4d_lcd_test.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(SECONDARY_FLASH)$(EXECUTABLES)$(SECONDARY_LIST)$(S_UPPER_DEPS) stm32f4d_lcd_test.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
