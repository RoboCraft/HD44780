################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/core_cm3.c \
../CMSIS/system_stm32f2xx.c 

S_UPPER_SRCS += \
../CMSIS/startup_stm32f2xx.S 

OBJS += \
./CMSIS/core_cm3.o \
./CMSIS/startup_stm32f2xx.o \
./CMSIS/system_stm32f2xx.o 

C_DEPS += \
./CMSIS/core_cm3.d \
./CMSIS/system_stm32f2xx.d 

S_UPPER_DEPS += \
./CMSIS/startup_stm32f2xx.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/%.o: ../CMSIS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC C Compiler'
	arm-none-eabi-gcc -DUSE_STDPERIPH_DRIVER -I"/home/burjui/devel/stm32/stm32f2xx_lcd_test/CMSIS" -I"/home/burjui/devel/stm32/stm32f2xx_lcd_test/StdPeripheralDriver/inc" -O0 -ffunction-sections -fdata-sections -Wall -std=c99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g3 -gdwarf-2 -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CMSIS/%.o: ../CMSIS/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC Assembler'
	arm-none-eabi-gcc -x assembler-with-cpp -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g3 -gdwarf-2 -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


