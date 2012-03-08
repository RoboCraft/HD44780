################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../StdPeripheralDriver/src/misc.c \
../StdPeripheralDriver/src/stm32l1xx_adc.c \
../StdPeripheralDriver/src/stm32l1xx_aes.c \
../StdPeripheralDriver/src/stm32l1xx_aes_util.c \
../StdPeripheralDriver/src/stm32l1xx_comp.c \
../StdPeripheralDriver/src/stm32l1xx_crc.c \
../StdPeripheralDriver/src/stm32l1xx_dac.c \
../StdPeripheralDriver/src/stm32l1xx_dbgmcu.c \
../StdPeripheralDriver/src/stm32l1xx_dma.c \
../StdPeripheralDriver/src/stm32l1xx_exti.c \
../StdPeripheralDriver/src/stm32l1xx_flash.c \
../StdPeripheralDriver/src/stm32l1xx_flash_ramfunc.c \
../StdPeripheralDriver/src/stm32l1xx_fsmc.c \
../StdPeripheralDriver/src/stm32l1xx_gpio.c \
../StdPeripheralDriver/src/stm32l1xx_i2c.c \
../StdPeripheralDriver/src/stm32l1xx_iwdg.c \
../StdPeripheralDriver/src/stm32l1xx_lcd.c \
../StdPeripheralDriver/src/stm32l1xx_opamp.c \
../StdPeripheralDriver/src/stm32l1xx_pwr.c \
../StdPeripheralDriver/src/stm32l1xx_rcc.c \
../StdPeripheralDriver/src/stm32l1xx_rtc.c \
../StdPeripheralDriver/src/stm32l1xx_sdio.c \
../StdPeripheralDriver/src/stm32l1xx_spi.c \
../StdPeripheralDriver/src/stm32l1xx_syscfg.c \
../StdPeripheralDriver/src/stm32l1xx_tim.c \
../StdPeripheralDriver/src/stm32l1xx_usart.c \
../StdPeripheralDriver/src/stm32l1xx_wwdg.c 

OBJS += \
./StdPeripheralDriver/src/misc.o \
./StdPeripheralDriver/src/stm32l1xx_adc.o \
./StdPeripheralDriver/src/stm32l1xx_aes.o \
./StdPeripheralDriver/src/stm32l1xx_aes_util.o \
./StdPeripheralDriver/src/stm32l1xx_comp.o \
./StdPeripheralDriver/src/stm32l1xx_crc.o \
./StdPeripheralDriver/src/stm32l1xx_dac.o \
./StdPeripheralDriver/src/stm32l1xx_dbgmcu.o \
./StdPeripheralDriver/src/stm32l1xx_dma.o \
./StdPeripheralDriver/src/stm32l1xx_exti.o \
./StdPeripheralDriver/src/stm32l1xx_flash.o \
./StdPeripheralDriver/src/stm32l1xx_flash_ramfunc.o \
./StdPeripheralDriver/src/stm32l1xx_fsmc.o \
./StdPeripheralDriver/src/stm32l1xx_gpio.o \
./StdPeripheralDriver/src/stm32l1xx_i2c.o \
./StdPeripheralDriver/src/stm32l1xx_iwdg.o \
./StdPeripheralDriver/src/stm32l1xx_lcd.o \
./StdPeripheralDriver/src/stm32l1xx_opamp.o \
./StdPeripheralDriver/src/stm32l1xx_pwr.o \
./StdPeripheralDriver/src/stm32l1xx_rcc.o \
./StdPeripheralDriver/src/stm32l1xx_rtc.o \
./StdPeripheralDriver/src/stm32l1xx_sdio.o \
./StdPeripheralDriver/src/stm32l1xx_spi.o \
./StdPeripheralDriver/src/stm32l1xx_syscfg.o \
./StdPeripheralDriver/src/stm32l1xx_tim.o \
./StdPeripheralDriver/src/stm32l1xx_usart.o \
./StdPeripheralDriver/src/stm32l1xx_wwdg.o 

C_DEPS += \
./StdPeripheralDriver/src/misc.d \
./StdPeripheralDriver/src/stm32l1xx_adc.d \
./StdPeripheralDriver/src/stm32l1xx_aes.d \
./StdPeripheralDriver/src/stm32l1xx_aes_util.d \
./StdPeripheralDriver/src/stm32l1xx_comp.d \
./StdPeripheralDriver/src/stm32l1xx_crc.d \
./StdPeripheralDriver/src/stm32l1xx_dac.d \
./StdPeripheralDriver/src/stm32l1xx_dbgmcu.d \
./StdPeripheralDriver/src/stm32l1xx_dma.d \
./StdPeripheralDriver/src/stm32l1xx_exti.d \
./StdPeripheralDriver/src/stm32l1xx_flash.d \
./StdPeripheralDriver/src/stm32l1xx_flash_ramfunc.d \
./StdPeripheralDriver/src/stm32l1xx_fsmc.d \
./StdPeripheralDriver/src/stm32l1xx_gpio.d \
./StdPeripheralDriver/src/stm32l1xx_i2c.d \
./StdPeripheralDriver/src/stm32l1xx_iwdg.d \
./StdPeripheralDriver/src/stm32l1xx_lcd.d \
./StdPeripheralDriver/src/stm32l1xx_opamp.d \
./StdPeripheralDriver/src/stm32l1xx_pwr.d \
./StdPeripheralDriver/src/stm32l1xx_rcc.d \
./StdPeripheralDriver/src/stm32l1xx_rtc.d \
./StdPeripheralDriver/src/stm32l1xx_sdio.d \
./StdPeripheralDriver/src/stm32l1xx_spi.d \
./StdPeripheralDriver/src/stm32l1xx_syscfg.d \
./StdPeripheralDriver/src/stm32l1xx_tim.d \
./StdPeripheralDriver/src/stm32l1xx_usart.d \
./StdPeripheralDriver/src/stm32l1xx_wwdg.d 


# Each subdirectory must supply rules for building sources it contributes
StdPeripheralDriver/src/%.o: ../StdPeripheralDriver/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC C Compiler'
	arm-none-eabi-gcc -DSTM32L1XX_MD -DUSE_STDPERIPH_DRIVER -I"/home/burjui/devel/stm32/stm32ld_lcd_test/CMSIS" -I"/home/burjui/devel/stm32/stm32ld_lcd_test/StdPeripheralDriver/inc" -O0 -ffunction-sections -fdata-sections -Wall -std=c99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -g3 -gdwarf-2 -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


