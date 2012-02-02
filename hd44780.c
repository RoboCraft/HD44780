#include "hd44780.h"

#ifdef HD44780_USE_STM32F10X
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#endif

void HD44780_HANG_ASSERT(int x)
{
  if (!x)
    do __NOP(); while (1);
}

#ifndef NDEBUG
  #define HD44780_RETURN_ASSERT(x,ret) \
    HD44780_HANG_ASSERT(x)
#else
  #define HD44780_RETURN_ASSERT(x,ret) \
    do { if (!(x)) return (ret); while (1); } while (0)
#endif


HD44780_Result hd44780_init(HD44780 *display, HD44780_Mode mode,
    const HD44780_HAL *hal, const HD44780_Pinout *pinout,
    uint8_t cols, uint8_t lines, uint8_t charsize)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(hal != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(hal->pins.configure != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(hal->pins.write != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(hal->delay_microseconds != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(pinout != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(cols > 0, HD44780_ERROR);
  HD44780_RETURN_ASSERT(lines > 0, HD44780_ERROR);

  /* For some 1 line displays you can select a 10 pixel high font */
  if ((charsize != 0) && (lines == 1))
    display->displayfunction |= LCD_5x10DOTS;

  display->hal = *hal;
  display->pinout = *pinout;

  display->hal.pins.configure(&display->pinout.rs, HD44780_PIN_OUTPUT);
  display->hal.pins.configure(&display->pinout.enable, HD44780_PIN_OUTPUT);

  if (display->pinout.rw.gpio != NULL)
    display->hal.pins.configure(&display->pinout.rw, HD44780_PIN_OUTPUT);

  if (display->pinout.backlight.gpio != NULL)
  {
    display->hal.pins.configure(&display->pinout.backlight, HD44780_PIN_OUTPUT);
    display->hal.pins.write(&display->pinout.backlight, HD44780_PIN_LOW);
  }

  if (mode == HD44780_MODE_4BIT)
  {
    display->displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    display->dp_offset = 4;
    display->dp_amount = 4;
  }
  else
  {
    display->displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
    display->dp_offset = 0;
    display->dp_amount = 8;
  }

  if (lines > 1)
    display->displayfunction |= LCD_2LINE;

  display->numlines = lines;
  display->currline = 0;

  if (hd44780_config(display) != HD44780_OK)
    return HD44780_ERROR;

  /* Put the LCD into 4 bit or 8 bit mode */
  if (! (display->displayfunction & LCD_8BITMODE))
  {
    /* This is according to the hitachi HD44780 datasheet figure 24, pg 46 */

    /* We start in 8bit mode, try to set 4 bit mode */
    hd44780_write4bits(display, 0x03);
    display->hal.delay_microseconds(4500); // wait min 4.1ms

    /* Second try */
    hd44780_write4bits(display, 0x03);
    display->hal.delay_microseconds(4500); // wait min 4.1ms

    /* Third go! */
    hd44780_write4bits(display, 0x03);
    display->hal.delay_microseconds(150);

    /* Finally, set to 4-bit interface */
    hd44780_write4bits(display, 0x02);
  }
  else
  {
    /* This is according to the hitachi HD44780 datasheet page 45 figure 23 */

    /* Send function set command sequence */
    hd44780_command(display, LCD_FUNCTIONSET | display->displayfunction);
    display->hal.delay_microseconds(4500);  // wait more than 4.1ms

    /* Second try */
    hd44780_command(display, LCD_FUNCTIONSET | display->displayfunction);
    display->hal.delay_microseconds(150);

    /* Third go */
    hd44780_command(display, LCD_FUNCTIONSET | display->displayfunction);
  }

  /* Finally, set # lines, font size, etc. */
  hd44780_command(display, LCD_FUNCTIONSET | display->displayfunction);

  /* Turn the display on with no cursor or blinking default */
  display->displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  hd44780_display_on(display);

  /* Clear it off */
  hd44780_clear(display);

  /* Initialize to default text direction (for romance languages) */
  display->displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

  /* Set the entry mode */
  hd44780_command(display, LCD_ENTRYMODESET | display->displaymode);

  return HD44780_OK;
}

HD44780_Result hd44780_write(HD44780 *display, uint8_t value)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  hd44780_send(display, value, SET);

  return HD44780_OK;
}

HD44780_Result hd44780_clear(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.delay_microseconds != NULL, HD44780_ERROR);

  hd44780_command(display, LCD_CLEARDISPLAY); // clear display, set cursor position to zero
  display->hal.delay_microseconds(2000); // this command takes a long time!

  return HD44780_OK;
}

HD44780_Result hd44780_home(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.delay_microseconds != NULL, HD44780_ERROR);

  hd44780_command(display, LCD_RETURNHOME);  // set cursor position to zero
  display->hal.delay_microseconds(2000);  // this command takes a long time!

  return HD44780_OK;
}

HD44780_Result hd44780_display_off(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  display->displaycontrol &= ~LCD_DISPLAYON;
  hd44780_command(display, LCD_DISPLAYCONTROL | display->displaycontrol);

  return HD44780_OK;
}

HD44780_Result hd44780_display_on(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  display->displaycontrol |= LCD_DISPLAYON;
  hd44780_command(display, LCD_DISPLAYCONTROL | display->displaycontrol);

  return HD44780_OK;
}


HD44780_Result hd44780_noBlink(HD44780 *display);
HD44780_Result hd44780_blink(HD44780 *display);
HD44780_Result hd44780_noCursor(HD44780 *display);
HD44780_Result hd44780_cursor(HD44780 *display);
HD44780_Result hd44780_scrollDisplayLeft(HD44780 *display);
HD44780_Result hd44780_scrollDisplayRight(HD44780 *display);
HD44780_Result hd44780_leftToRight(HD44780 *display);
HD44780_Result hd44780_rightToLeft(HD44780 *display);
HD44780_Result hd44780_autoscroll(HD44780 *display);
HD44780_Result hd44780_noAutoscroll(HD44780 *display);
HD44780_Result hd44780_createChar(HD44780 *display, uint8_t location, uint8_t *charmap);
HD44780_Result hd44780_setCursor(HD44780 *display, uint8_t col, uint8_t row);
HD44780_Result hd44780_command(HD44780 *display, uint8_t value);

HD44780_Result hd44780_config(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.delay_microseconds != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.pins.configure != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.pins.write != NULL, HD44780_ERROR);

  for (unsigned i = 0; i < display->dp_amount; ++i)
  {
    display->hal.pins.configure(&display->pinout.dp[display->dp_offset + i], HD44780_PIN_OUTPUT);
    display->hal.pins.write(&display->pinout.dp[display->dp_offset + i], HD44780_PIN_LOW);
  }

  /* SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
   * according to datasheet, we need at least 40ms after power rises above 2.7V
   * before sending commands.
   */
  display->hal.delay_microseconds(50000);

  /* Now we pull both RS and R/W low to begin commands */
  display->hal.pins.write(&display->pinout.rs, HD44780_PIN_LOW);
  display->hal.pins.write(&display->pinout.enable, HD44780_PIN_LOW);

  if (display->pinout.rw.gpio != NULL)
    display->hal.pins.write(&display->pinout.rw, HD44780_PIN_LOW);

  return HD44780_OK;
}

HD44780_Result hd44780_command(HD44780 *display, uint8_t value)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  hd44780_send(display, value, RESET);

  return HD44780_OK;
}

HD44780_Result hd44780_send(HD44780 *display, uint8_t value, uint8_t mode)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.pins.write != NULL, HD44780_ERROR);

  display->hal.pins.write(&display->pinout.rs, mode);

  /* If there is a RW pin indicated, set it low to write */
  if (display->pinout.rw.gpio != NULL)
    display->hal.pins.write(&display->pinout.rw, HD44780_PIN_LOW);

  if (display->displayfunction & LCD_8BITMODE)
    hd44780_write8bits(display, value);
  else
  {
    hd44780_write4bits(display, value >> 4);
    hd44780_write4bits(display, value);
  }

  return HD44780_OK;
}

HD44780_Result hd44780_write8bits(HD44780 *display, uint8_t value)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.pins.write != NULL, HD44780_ERROR);

  for (unsigned i = 0; i < display->dp_amount; ++i)
  {
    /* FIXME Use this if you implement reading which will use data pins as inputs:
             display->hal.pins.configure(&display->pinout.dp[i]) */
    display->hal.pins.write(&display->pinout.dp[display->dp_offset + i], (value >> i) & 0x01);
  }

  hd44780_pulse_enable_pin(display);

  return HD44780_OK;
}

HD44780_Result hd44780_write4bits(HD44780 *display, uint8_t value)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.pins.write != NULL, HD44780_ERROR);

  for (unsigned i = 0; i < display->dp_amount; ++i)
  {
    /* FIXME Use this if you implement reading which will use data pins as inputs:
             display->hal.pins.configure(&display->pinout.dp[i]) */
    display->hal.pins.write(&display->pinout.dp[display->dp_offset + i], (value >> i) & 0x01);
  }

  hd44780_pulse_enable_pin(display);

  return HD44780_OK;
}

HD44780_Result hd44780_pulse_enable_pin(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.pins.write != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.delay_microseconds != NULL, HD44780_ERROR);

  display->hal.pins.write(&display->pinout.enable, HD44780_PIN_LOW);
  display->hal.delay_microseconds(1);
  display->hal.pins.write(&display->pinout.enable, HD44780_PIN_HIGH);
  display->hal.delay_microseconds(1); // enable pulse must be >450ns
  display->hal.pins.write(&display->pinout.enable, HD44780_PIN_LOW);
  display->hal.delay_microseconds(100); // commands need > 37us to settle

  return HD44780_OK;
}


/***** PinDriver for STM32F10X *****/

#ifdef HD44780_USE_STM32F10X

HD44780_Result stm32f10x_default_pin_configure(HD44780_Pin *pin, HD44780_PinMode mode)
{
  HD44780_RETURN_ASSERT(pin != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(pin->gpio != NULL, HD44780_ERROR);

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

    default: HD44780_HANG_ASSERT(0);
  }

  gpio_config.GPIO_Pin = pin->gpio_pin;
  GPIO_Init(pin->gpio, &gpio_config);

  return HD44780_OK;
}

HD44780_Result stm32f10x_default_pin_write(HD44780_Pin *pin, HD44780_PinState value)
{
  HD44780_RETURN_ASSERT(pin != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(pin->gpio != NULL, HD44780_ERROR);

  GPIO_WriteBit(pin->gpio, pin->gpio_pin, (value == HD44780_PIN_LOW ? RESET : SET));

  return HD44780_OK;
}

const PinDriver STM32F10X_PinDriver =
{
  stm32f10x_default_pin_configure,
  stm32f10x_default_pin_write,
  // stm32f10x_default_pin_read
};

#endif /* HD44780_USE_STM32F10X */
