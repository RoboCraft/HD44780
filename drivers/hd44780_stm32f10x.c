#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stdint.h>
#include <stdlib.h>
#include "hd44780_stm32f10x.h"


#ifndef NDEBUG
  #define HD44780_STM32F10X_ASSERT(x) \
    { \
      if (!(x)) \
      { \
        HD44780_STM32F10x_GPIO_Driver *stm32f10x_driver = ((HD44780_STM32F10x_GPIO_Driver*)driver); \
        if (stm32f10x_driver->assert_failure_handler != NULL) \
          stm32f10x_driver->assert_failure_handler(__FILE__, __LINE__); \
      } \
    }

  #define HD44780_STM32F10X_RETURN_ASSERT(x,ret) \
    do { \
      int condition = (x); \
      HD44780_STM32F10X_ASSERT(condition) \
      if (!condition) \
        return (ret); \
    } while (0)
#else
  #define HD44780_STM32F10X_ASSERT(x)
  #define HD44780_STM32F10X_RETURN_ASSERT(x,ret)
#endif


static HD44780_Result stm32f10x_default_pin_configure(HD44780_GPIO_Interface *driver,
    HD44780_Pin pin, HD44780_PinMode mode)
{
  HD44780_STM32F10X_RETURN_ASSERT(driver != NULL, HD44780_ERROR);

  HD44780_STM32F10x_GPIO_Driver *stm32f10x_pindriver = (HD44780_STM32F10x_GPIO_Driver*)driver;
  HD44780_STM32F10x_Pin *stm32f10x_pin = &stm32f10x_pindriver->pinout.pins[pin];

  HD44780_STM32F10X_RETURN_ASSERT(stm32f10x_pin->gpio != NULL, HD44780_ERROR);

  GPIO_InitTypeDef gpio_config;
  GPIO_StructInit(&gpio_config);

  switch (mode)
  {
    case HD44780_PIN_OUTPUT:
      gpio_config.GPIO_Mode = GPIO_Mode_Out_PP;
      break;

    case HD44780_PIN_INPUT:
      gpio_config.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      break;

    default: HD44780_STM32F10X_ASSERT(0);
  }

  gpio_config.GPIO_Pin = stm32f10x_pin->pinmask;

  if (stm32f10x_pin->gpio != NULL)
    GPIO_Init(stm32f10x_pin->gpio, &gpio_config);

  return HD44780_OK;
}

static HD44780_Result stm32f10x_default_pin_write(HD44780_GPIO_Interface *driver,
    HD44780_Pin pin, HD44780_PinState value)
{
  HD44780_STM32F10X_RETURN_ASSERT(driver != NULL, HD44780_ERROR);

  HD44780_STM32F10x_GPIO_Driver *stm32f10x_pindriver = (HD44780_STM32F10x_GPIO_Driver*)driver;
  HD44780_STM32F10x_Pin *stm32f10x_pin = &stm32f10x_pindriver->pinout.pins[pin];

  HD44780_STM32F10X_RETURN_ASSERT(stm32f10x_pin->gpio != NULL, HD44780_ERROR);

  if (stm32f10x_pin->gpio != NULL)
    GPIO_WriteBit(stm32f10x_pin->gpio, stm32f10x_pin->pinmask, (value == HD44780_PIN_LOW ? RESET : SET));

  return HD44780_OK;
}

static HD44780_Result stm32f10x_default_pin_read(HD44780_GPIO_Interface *driver,
    HD44780_Pin pin, HD44780_PinState *value)
{
  HD44780_STM32F10X_RETURN_ASSERT(driver != NULL, HD44780_ERROR);
  HD44780_STM32F10X_RETURN_ASSERT(value != NULL, HD44780_ERROR);

  HD44780_STM32F10x_GPIO_Driver *stm32f10x_pindriver = (HD44780_STM32F10x_GPIO_Driver*)driver;
  HD44780_STM32F10x_Pin *stm32f10x_pin = &stm32f10x_pindriver->pinout.pins[pin];

  HD44780_STM32F10X_RETURN_ASSERT(stm32f10x_pin->gpio != NULL, HD44780_ERROR);

  if (stm32f10x_pin->gpio != NULL)
  {
    *value = (GPIO_ReadInputDataBit(stm32f10x_pin->gpio, stm32f10x_pin->pinmask) == RESET ?
        HD44780_PIN_LOW : HD44780_PIN_HIGH);
  }
  else
    *value = HD44780_PIN_LOW;

  return HD44780_OK;
}

const HD44780_GPIO_Interface HD44780_STM32F10X_PINDRIVER_INTERFACE =
{
  stm32f10x_default_pin_configure,
  stm32f10x_default_pin_write,
  stm32f10x_default_pin_read
};
