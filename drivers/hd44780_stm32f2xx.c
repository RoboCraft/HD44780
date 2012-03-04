#include <stm32f2xx_rcc.h>
#include <stm32f2xx_gpio.h>
#include <stdint.h>
#include <stdlib.h>
#include "hd44780_stm32f2xx.h"


#ifndef NDEBUG
  #define HD44780_STM32F2XX_ASSERT(x) \
    { \
      if (!(x)) \
      { \
        HD44780_STM32F2xx_GPIO_Driver *stm32f2xx_driver = ((HD44780_STM32F2xx_GPIO_Driver*)driver); \
        if (stm32f2xx_driver->assert_failure_handler != NULL) \
          stm32f2xx_driver->assert_failure_handler(__FILE__, __LINE__); \
      } \
    }

  #define HD44780_STM32F2XX_RETURN_ASSERT(x,ret) \
    do { \
      int condition = (x); \
      HD44780_STM32F2XX_ASSERT(condition) \
      if (!condition) \
        return (ret); \
    } while (0)
#else
  #define HD44780_STM32F2XX_ASSERT(x)
  #define HD44780_STM32F2XX_RETURN_ASSERT(x,ret)
#endif


static HD44780_Result stm32f2xx_default_pin_configure(HD44780_GPIO_Interface *driver,
    HD44780_Pin pin, HD44780_PinMode mode)
{
  HD44780_STM32F2XX_RETURN_ASSERT(driver != NULL, HD44780_RESULT_ERROR);

  HD44780_STM32F2xx_GPIO_Driver *stm32f2xx_pindriver = (HD44780_STM32F2xx_GPIO_Driver*)driver;
  HD44780_STM32F2xx_Pin *stm32f2xx_pin = &stm32f2xx_pindriver->pinout.pins[pin];

  HD44780_STM32F2XX_RETURN_ASSERT(stm32f2xx_pin->gpio != NULL, HD44780_RESULT_ERROR);

  GPIO_InitTypeDef gpio_config;
  GPIO_StructInit(&gpio_config);

  switch (mode)
  {
    case HD44780_PINMODE_OUTPUT:
      gpio_config.GPIO_Mode = GPIO_Mode_OUT;
      break;

    case HD44780_PINMODE_INPUT:
      gpio_config.GPIO_Mode = GPIO_Mode_IN;
      break;

    default:
      HD44780_STM32F2XX_ASSERT(0);
      break;
  }

  gpio_config.GPIO_Pin = stm32f2xx_pin->pinmask;

  if (stm32f2xx_pin->gpio != NULL)
    GPIO_Init(stm32f2xx_pin->gpio, &gpio_config);

  return HD44780_RESULT_OK;
}

static HD44780_Result stm32f2xx_default_pin_write(HD44780_GPIO_Interface *driver,
    HD44780_Pin pin, HD44780_PinState value)
{
  HD44780_STM32F2XX_RETURN_ASSERT(driver != NULL, HD44780_RESULT_ERROR);

  HD44780_STM32F2xx_GPIO_Driver *stm32f2xx_pindriver = (HD44780_STM32F2xx_GPIO_Driver*)driver;
  HD44780_STM32F2xx_Pin *stm32f2xx_pin = &stm32f2xx_pindriver->pinout.pins[pin];

  HD44780_STM32F2XX_RETURN_ASSERT(stm32f2xx_pin->gpio != NULL, HD44780_RESULT_ERROR);

  if (stm32f2xx_pin->gpio != NULL)
  {
    GPIO_WriteBit(stm32f2xx_pin->gpio, stm32f2xx_pin->pinmask,
      (value == HD44780_PINSTATE_LOW ? Bit_RESET : Bit_SET));
  }

  return HD44780_RESULT_OK;
}

static HD44780_Result stm32f2xx_default_pin_read(HD44780_GPIO_Interface *driver,
    HD44780_Pin pin, HD44780_PinState *value)
{
  HD44780_STM32F2XX_RETURN_ASSERT(driver != NULL, HD44780_RESULT_ERROR);
  HD44780_STM32F2XX_RETURN_ASSERT(value != NULL, HD44780_RESULT_ERROR);

  HD44780_STM32F2xx_GPIO_Driver *stm32f2xx_pindriver = (HD44780_STM32F2xx_GPIO_Driver*)driver;
  HD44780_STM32F2xx_Pin *stm32f2xx_pin = &stm32f2xx_pindriver->pinout.pins[pin];

  HD44780_STM32F2XX_RETURN_ASSERT(stm32f2xx_pin->gpio != NULL, HD44780_RESULT_ERROR);

  uint8_t out_bit = GPIO_ReadInputDataBit(stm32f2xx_pin->gpio, stm32f2xx_pin->pinmask);
  *value = (out_bit == Bit_RESET ? HD44780_PINSTATE_LOW : HD44780_PINSTATE_HIGH);

  return HD44780_RESULT_OK;
}

const HD44780_GPIO_Interface HD44780_STM32F2XX_PINDRIVER_INTERFACE =
{
  stm32f2xx_default_pin_configure,
  stm32f2xx_default_pin_write,
  stm32f2xx_default_pin_read
};
