/*******************************************************************************
  * @file      startup_stm32f10x_md_vl.s
  * @author    MCD Application Team
  * @version   V3.5.0
  * @date      11-March-2011
  * @brief     STM32F10x Medium Density Value Line Devices vector table for RIDE7
  *            toolchain.
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address
  *                - Configure the clock system
  *                - Branches to main in the C library (which eventually
  *                  calls main()).
  *            After Reset the Cortex-M3 processor is in Thread mode,
  *            priority is Privileged, and the Stack is set to Main.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************/

  .syntax unified
  .cpu cortex-m3
  .fpu softvfp
  .thumb

.global  g_pfnVectors
.global  Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss

.equ  BootRAM, 0xF108F85F
/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval None
*/

  .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:

/* Copy the data segment initializers from flash to SRAM */
  movs  r1, #0
  b     LoopCopyDataInit

CopyDataInit:
  ldr   r3, =_sidata
  ldr   r3, [r3, r1]
  str   r3, [r0, r1]
  adds  r1, r1, #4

LoopCopyDataInit:
  ldr   r0, =_sdata
  ldr   r3, =_edata
  adds  r2, r0, r1
  cmp   r2, r3
  bcc   CopyDataInit
  ldr   r2, =_sbss
  b     LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
  movs  r3, #0
  str   r3, [r2], #4

LoopFillZerobss:
  ldr   r3, = _ebss
  cmp   r2, r3
  bcc   FillZerobss
/* Call the clock system intitialization function.*/
  bl    SystemInit
/* Call the application's entry point.*/
  bl    main
  bx    lr
.size   Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt. This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 * @param  None
 * @retval None
*/
  .section  .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b  Infinite_Loop
  .size  Default_Handler, .-Default_Handler


.macro define_hangler original_name,stub_name
  .section .text.\stub_name,"ax",%progbits
  \stub_name:
    // bl enable_error_led
    Infinite_Loop_\stub_name:
      b Infinite_Loop_\stub_name
  .size  \stub_name, .-\stub_name

  .weak \original_name
  .thumb_set \original_name,\stub_name
.endm

/******************************************************************************
* The minimal vector table for a Cortex M3. Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
******************************************************************************/
  .section  .isr_vector,"a",%progbits
  .type  g_pfnVectors, %object
  .size  g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word  _estack
  .word  Reset_Handler
  .word  NMI_Handler
  .word  HardFault_Handler
  .word  MemManage_Handler
  .word  BusFault_Handler
  .word  UsageFault_Handler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  SVC_Handler
  .word  DebugMon_Handler
  .word  0
  .word  PendSV_Handler
  .word  SysTick_Handler
  .word  WWDG_IRQHandler
  .word  PVD_IRQHandler
  .word  TAMPER_IRQHandler
  .word  RTC_IRQHandler
  .word  FLASH_IRQHandler
  .word  RCC_IRQHandler
  .word  EXTI0_IRQHandler
  .word  EXTI1_IRQHandler
  .word  EXTI2_IRQHandler
  .word  EXTI3_IRQHandler
  .word  EXTI4_IRQHandler
  .word  DMA1_Channel1_IRQHandler
  .word  DMA1_Channel2_IRQHandler
  .word  DMA1_Channel3_IRQHandler
  .word  DMA1_Channel4_IRQHandler
  .word  DMA1_Channel5_IRQHandler
  .word  DMA1_Channel6_IRQHandler
  .word  DMA1_Channel7_IRQHandler
  .word  ADC1_IRQHandler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  EXTI9_5_IRQHandler
  .word  TIM1_BRK_TIM15_IRQHandler
  .word  TIM1_UP_TIM16_IRQHandler
  .word  TIM1_TRG_COM_TIM17_IRQHandler
  .word  TIM1_CC_IRQHandler
  .word  TIM2_IRQHandler
  .word  TIM3_IRQHandler
  .word  TIM4_IRQHandler
  .word  I2C1_EV_IRQHandler
  .word  I2C1_ER_IRQHandler
  .word  I2C2_EV_IRQHandler
  .word  I2C2_ER_IRQHandler
  .word  SPI1_IRQHandler
  .word  SPI2_IRQHandler
  .word  USART1_IRQHandler
  .word  USART2_IRQHandler
  .word  USART3_IRQHandler
  .word  EXTI15_10_IRQHandler
  .word  RTCAlarm_IRQHandler
  .word  CEC_IRQHandler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  TIM6_DAC_IRQHandler
  .word  TIM7_IRQHandler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  BootRAM          /* @0x01CC. This is for boot in RAM mode for
                            STM32F10x Medium Value Line Density devices. */

/*******************************************************************************
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*******************************************************************************/

define_hangler NMI_Handler,NMI_Handler_Default
define_hangler HardFault_Handler,HardFault_Handler_Default
define_hangler MemManage_Handler,MemManage_Handler_Default
define_hangler BusFault_Handler,BusFault_Handler_Default
define_hangler UsageFault_Handler,UsageFault_Handler_Default
define_hangler SVC_Handler,SVC_Handler_Default
define_hangler DebugMon_Handler,DebugMon_Handler_Default
define_hangler PendSV_Handler,PendSV_Handler_Default
define_hangler SysTick_Handler,SysTick_Handler_Default
define_hangler WWDG_IRQHandler,WWDG_IRQHandler_Default
define_hangler PVD_IRQHandler,PVD_IRQHandler_Default
define_hangler TAMPER_IRQHandler,TAMPER_IRQHandler_Default
define_hangler RTC_IRQHandler,RTC_IRQHandler_Default
define_hangler FLASH_IRQHandler,FLASH_IRQHandler_Default
define_hangler RCC_IRQHandler,RCC_IRQHandler_Default
define_hangler EXTI0_IRQHandler,EXTI0_IRQHandler_Default
define_hangler EXTI1_IRQHandler,EXTI1_IRQHandler_Default
define_hangler EXTI2_IRQHandler,EXTI2_IRQHandler_Default
define_hangler EXTI3_IRQHandler,EXTI3_IRQHandler_Default
define_hangler EXTI4_IRQHandler,EXTI4_IRQHandler_Default
define_hangler DMA1_Channel1_IRQHandler,DMA1_Channel1_IRQHandler_Default
define_hangler DMA1_Channel2_IRQHandler,DMA1_Channel2_IRQHandler_Default
define_hangler DMA1_Channel3_IRQHandler,DMA1_Channel3_IRQHandler_Default
define_hangler DMA1_Channel4_IRQHandler,DMA1_Channel4_IRQHandler_Default
define_hangler DMA1_Channel5_IRQHandler,DMA1_Channel5_IRQHandler_Default
define_hangler DMA1_Channel6_IRQHandler,DMA1_Channel6_IRQHandler_Default
define_hangler DMA1_Channel7_IRQHandler,DMA1_Channel7_IRQHandler_Default
define_hangler ADC1_IRQHandler,ADC1_IRQHandler_Default
define_hangler EXTI9_5_IRQHandler,EXTI9_5_IRQHandler_Default
define_hangler TIM1_BRK_TIM15_IRQHandler,TIM1_BRK_TIM15_IRQHandler_Default
define_hangler TIM1_UP_TIM16_IRQHandler,TIM1_UP_TIM16_IRQHandler_Default
define_hangler TIM1_TRG_COM_TIM17_IRQHandler,TIM1_TRG_COM_TIM17_IRQHandler_Default
define_hangler TIM1_CC_IRQHandler,TIM1_CC_IRQHandler_Default
define_hangler TIM2_IRQHandler,TIM2_IRQHandler_Default
define_hangler TIM3_IRQHandler,TIM3_IRQHandler_Default
define_hangler TIM4_IRQHandler,TIM4_IRQHandler_Default
define_hangler I2C1_EV_IRQHandler,I2C1_EV_IRQHandler_Default
define_hangler I2C1_ER_IRQHandler,I2C1_ER_IRQHandler_Default
define_hangler I2C2_EV_IRQHandler,I2C2_EV_IRQHandler_Default
define_hangler I2C2_ER_IRQHandler,I2C2_ER_IRQHandler_Default
define_hangler SPI1_IRQHandler,SPI1_IRQHandler_Default
define_hangler SPI2_IRQHandler,SPI2_IRQHandler_Default
define_hangler USART1_IRQHandler,USART1_IRQHandler_Default
define_hangler USART2_IRQHandler,USART2_IRQHandler_Default
define_hangler USART3_IRQHandler,USART3_IRQHandler_Default
define_hangler EXTI15_10_IRQHandler,EXTI15_10_IRQHandler_Default
define_hangler RTCAlarm_IRQHandler,RTCAlarm_IRQHandler_Default
define_hangler CEC_IRQHandler,CEC_IRQHandler_Default
define_hangler TIM6_DAC_IRQHandler,TIM6_DAC_IRQHandler_Default
define_hangler TIM7_IRQHandler,TIM7_IRQHandler_Default

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
