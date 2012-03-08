################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hd44780.c \
../hd44780_stm32f10x.c \
../main.c \
../newlib_stubs.c \
../utils.c 

S_UPPER_SRCS += \
../test.S 

OBJS += \
./hd44780.o \
./hd44780_stm32f10x.o \
./main.o \
./newlib_stubs.o \
./test.o \
./utils.o 

C_DEPS += \
./hd44780.d \
./hd44780_stm32f10x.d \
./main.d \
./newlib_stubs.d \
./utils.d 

S_UPPER_DEPS += \
./test.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC C Compiler'
	arm-none-eabi-gcc -DSTM32F10X_MD_VL -DUSE_FULL_ASSERT -DUSE_STDPERIPH_DRIVER -I"/home/burjui/devel/stm32/stm32vld_lcd_test/CMSIS" -I"/home/burjui/devel/stm32/stm32vld_lcd_test/StdPeripheralDriver/inc" -O0 -ffunction-sections -fdata-sections -Wall -Wextra -std=c99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -g3 -gdwarf-2 -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.S
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC Assembler'
	arm-none-eabi-gcc -x assembler-with-cpp -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -g3 -gdwarf-2 -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


