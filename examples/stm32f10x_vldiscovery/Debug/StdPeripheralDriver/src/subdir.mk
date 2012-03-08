################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../StdPeripheralDriver/src/misc.c \
../StdPeripheralDriver/src/stm32f10x_adc.c \
../StdPeripheralDriver/src/stm32f10x_bkp.c \
../StdPeripheralDriver/src/stm32f10x_can.c \
../StdPeripheralDriver/src/stm32f10x_cec.c \
../StdPeripheralDriver/src/stm32f10x_crc.c \
../StdPeripheralDriver/src/stm32f10x_dac.c \
../StdPeripheralDriver/src/stm32f10x_dbgmcu.c \
../StdPeripheralDriver/src/stm32f10x_dma.c \
../StdPeripheralDriver/src/stm32f10x_exti.c \
../StdPeripheralDriver/src/stm32f10x_flash.c \
../StdPeripheralDriver/src/stm32f10x_fsmc.c \
../StdPeripheralDriver/src/stm32f10x_gpio.c \
../StdPeripheralDriver/src/stm32f10x_i2c.c \
../StdPeripheralDriver/src/stm32f10x_iwdg.c \
../StdPeripheralDriver/src/stm32f10x_pwr.c \
../StdPeripheralDriver/src/stm32f10x_rcc.c \
../StdPeripheralDriver/src/stm32f10x_rtc.c \
../StdPeripheralDriver/src/stm32f10x_sdio.c \
../StdPeripheralDriver/src/stm32f10x_spi.c \
../StdPeripheralDriver/src/stm32f10x_tim.c \
../StdPeripheralDriver/src/stm32f10x_usart.c \
../StdPeripheralDriver/src/stm32f10x_wwdg.c 

OBJS += \
./StdPeripheralDriver/src/misc.o \
./StdPeripheralDriver/src/stm32f10x_adc.o \
./StdPeripheralDriver/src/stm32f10x_bkp.o \
./StdPeripheralDriver/src/stm32f10x_can.o \
./StdPeripheralDriver/src/stm32f10x_cec.o \
./StdPeripheralDriver/src/stm32f10x_crc.o \
./StdPeripheralDriver/src/stm32f10x_dac.o \
./StdPeripheralDriver/src/stm32f10x_dbgmcu.o \
./StdPeripheralDriver/src/stm32f10x_dma.o \
./StdPeripheralDriver/src/stm32f10x_exti.o \
./StdPeripheralDriver/src/stm32f10x_flash.o \
./StdPeripheralDriver/src/stm32f10x_fsmc.o \
./StdPeripheralDriver/src/stm32f10x_gpio.o \
./StdPeripheralDriver/src/stm32f10x_i2c.o \
./StdPeripheralDriver/src/stm32f10x_iwdg.o \
./StdPeripheralDriver/src/stm32f10x_pwr.o \
./StdPeripheralDriver/src/stm32f10x_rcc.o \
./StdPeripheralDriver/src/stm32f10x_rtc.o \
./StdPeripheralDriver/src/stm32f10x_sdio.o \
./StdPeripheralDriver/src/stm32f10x_spi.o \
./StdPeripheralDriver/src/stm32f10x_tim.o \
./StdPeripheralDriver/src/stm32f10x_usart.o \
./StdPeripheralDriver/src/stm32f10x_wwdg.o 

C_DEPS += \
./StdPeripheralDriver/src/misc.d \
./StdPeripheralDriver/src/stm32f10x_adc.d \
./StdPeripheralDriver/src/stm32f10x_bkp.d \
./StdPeripheralDriver/src/stm32f10x_can.d \
./StdPeripheralDriver/src/stm32f10x_cec.d \
./StdPeripheralDriver/src/stm32f10x_crc.d \
./StdPeripheralDriver/src/stm32f10x_dac.d \
./StdPeripheralDriver/src/stm32f10x_dbgmcu.d \
./StdPeripheralDriver/src/stm32f10x_dma.d \
./StdPeripheralDriver/src/stm32f10x_exti.d \
./StdPeripheralDriver/src/stm32f10x_flash.d \
./StdPeripheralDriver/src/stm32f10x_fsmc.d \
./StdPeripheralDriver/src/stm32f10x_gpio.d \
./StdPeripheralDriver/src/stm32f10x_i2c.d \
./StdPeripheralDriver/src/stm32f10x_iwdg.d \
./StdPeripheralDriver/src/stm32f10x_pwr.d \
./StdPeripheralDriver/src/stm32f10x_rcc.d \
./StdPeripheralDriver/src/stm32f10x_rtc.d \
./StdPeripheralDriver/src/stm32f10x_sdio.d \
./StdPeripheralDriver/src/stm32f10x_spi.d \
./StdPeripheralDriver/src/stm32f10x_tim.d \
./StdPeripheralDriver/src/stm32f10x_usart.d \
./StdPeripheralDriver/src/stm32f10x_wwdg.d 


# Each subdirectory must supply rules for building sources it contributes
StdPeripheralDriver/src/%.o: ../StdPeripheralDriver/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC C Compiler'
	arm-none-eabi-gcc -DSTM32F10X_MD_VL -DUSE_FULL_ASSERT -DUSE_STDPERIPH_DRIVER -I"/home/burjui/devel/stm32/stm32vld_lcd_test/CMSIS" -I"/home/burjui/devel/stm32/stm32vld_lcd_test/StdPeripheralDriver/inc" -O0 -ffunction-sections -fdata-sections -Wall -Wextra -std=c99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -g3 -gdwarf-2 -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


