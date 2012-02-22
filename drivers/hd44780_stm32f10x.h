#ifndef HC44780_STM32F10X_H_
#define HC44780_STM32F10X_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stdint.h>
#include "hd44780.h"

typedef struct
{
  GPIO_TypeDef *gpio;
  uint16_t pinmask;
} HD44780_STM32F10x_Pin;

typedef struct
{
  HD44780_STM32F10x_Pin pins[HD44780_PIN_AMOUNT];
} HD44780_STM32F10x_Pinout;

typedef enum
{
  HD44780_STM32F10X_ASSERT_NULL = 0x01
} HD44780_STM32F10x_PinDriverOptions;

typedef struct
{
  HD44780_PinDriver generic_driver;
  HD44780_STM32F10x_Pinout pinout;
  HD44780_STM32F10x_PinDriverOptions flags;
} HD44780_STM32F10x_PinDriver;

extern const HD44780_PinDriver HD44780_STM32F10X_DEFAULT_PINDRIVER;

#ifdef __cplusplus
}
#endif

#endif /* HC44780_STM32F10X_H_ */
