################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hd44780.c \
../hd44780_stm32f4xx.c \
../main.c \
../newlib_stubs.c 

OBJS += \
./hd44780.o \
./hd44780_stm32f4xx.o \
./main.o \
./newlib_stubs.o 

C_DEPS += \
./hd44780.d \
./hd44780_stm32f4xx.d \
./main.d \
./newlib_stubs.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC C Compiler'
	arm-none-eabi-gcc -DUSE_STDPERIPH_DRIVER -I"/home/burjui/devel/stm32/stm32f4d_lcd_test/CMSIS" -I"/home/burjui/devel/stm32/stm32f4d_lcd_test/StdPeripheralDriver/inc" -O0 -ffunction-sections -fdata-sections -Wall -std=c99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g3 -gdwarf-2 -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


