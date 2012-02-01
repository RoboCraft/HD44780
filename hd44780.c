#include "hd44780.h"

#ifdef HD44780_USE_STM32F10X
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#endif

HD44780_Result hd44780_init(HD44780 *display, HD44780_Mode mode,
    HD44780_HAL *hal,  HD44780_Pinout *pinout, uint8_t cols, uint8_t lines, uint8_t charsize)
{
  assert_param(display != NULL);
  assert_param(hal != NULL);
  assert_param(hal->pins.pin_config != NULL);
  assert_param(hal->pins.pin_write != NULL);
  assert_param(hal->delay_microseconds != NULL);
  assert_param(pinout != NULL);
  assert_param(cols > 0);
  assert_param(lines > 0);

  if (display == NULL || hal == NULL || hal->delay_microseconds == NULL ||
      pinout == NULL || cols == 0 || lines == 0)
    return 1;

  /* For some 1 line displays you can select a 10 pixel high font */
  if ((charsize != 0) && (lines == 1))
    display->displayfunction |= LCD_5x10DOTS;

  display->hal = *hal;

  display->rs_pin = pinout->rs_pin;
  display->hal.pins.pin_config(&display->rs_pin, HD44780_PIN_OUTPUT);

  display->rw_pin = pinout->rw_pin;

  if (display->rw_pin.gpio != NULL)
    display->hal.pins.pin_config(&display->rw_pin, HD44780_PIN_OUTPUT);

  display->enable_pin = pinout->enable_pin;
  display->hal.pins.pin_config(&display->enable_pin, HD44780_PIN_OUTPUT);

  display->backlight_pin = pinout->backlight_pin;

  if (display->backlight_pin.gpio != NULL)
  {
    display->hal.pins.pin_config(&display->backlight_pin, HD44780_PIN_OUTPUT);
    display->hal.pins.pin_write(&display->backlight_pin, HD44780_PIN_LOW);
  }

  for (int i = 0; i < 8; ++i)
    display->data_pins[i] = pinout->data_pins[i];

  if (mode == HD44780_MODE_4BIT)
    display->displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  else
    display->displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

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
  hd44780_display(display);

  /* Clear it off */
  hd44780_clear(display);

  /* Initialize to default text direction (for romance languages) */
  display->displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  /* Set the entry mode */
  hd44780_command(display, LCD_ENTRYMODESET | display->displaymode);

  return HD44780_OK;
}

HD44780_Result hd44780_clear(HD44780 *display)
{
  assert_param(display != NULL);
  assert_param(display->hal.delay_microseconds != NULL);

  if (display == NULL && display->hal.delay_microseconds == NULL)
    return HD44780_ERROR;

  hd44780_command(display, LCD_CLEARDISPLAY); // clear display, set cursor position to zero
  display->hal.delay_microseconds(2000); // this command takes a long time!

  return HD44780_OK;
}

void hd44780_home(HD44780 *display);

void hd44780_noDisplay(HD44780 *display)
{
  assert_param(display != NULL);

  display->displaycontrol &= ~LCD_DISPLAYON;
  hd44780_command(display, LCD_DISPLAYCONTROL | display->displaycontrol);
}

void hd44780_display(HD44780 *display)
{
  assert_param(display != NULL);

  display->displaycontrol |= LCD_DISPLAYON;
  hd44780_command(display, LCD_DISPLAYCONTROL | display->displaycontrol);
}


void hd44780_noBlink(HD44780 *display);
void hd44780_blink(HD44780 *display);
void hd44780_noCursor(HD44780 *display);
void hd44780_cursor(HD44780 *display);
void hd44780_scrollDisplayLeft(HD44780 *display);
void hd44780_scrollDisplayRight(HD44780 *display);
void hd44780_leftToRight(HD44780 *display);
void hd44780_rightToLeft(HD44780 *display);
void hd44780_autoscroll(HD44780 *display);
void hd44780_noAutoscroll(HD44780 *display);
void hd44780_createChar(HD44780 *display, uint8_t location, uint8_t *charmap);
void hd44780_setCursor(HD44780 *display, uint8_t col, uint8_t row);
void hd44780_command(HD44780 *display, uint8_t value);

HD44780_Result hd44780_config(HD44780 *display)
{
  assert_param(display != NULL);
  assert_param(display->hal.delay_microseconds != NULL);
  assert_param(display->hal.pins.pin_config != NULL);
  assert_param(display->hal.pins.pin_write != NULL);

  if (display == NULL || display->hal.delay_microseconds == NULL)
    return HD44780_ERROR;

  const int pin_amount = (display->displayfunction & LCD_8BITMODE ? 8 : 4);

  for (int i = 0; i < pin_amount; ++i)
  {
    display->hal.pins.pin_config(&display->data_pins[i], HD44780_PIN_OUTPUT);
    display->hal.pins.pin_write(&display->data_pins[i], HD44780_PIN_LOW);
  }

  /* SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
   * according to datasheet, we need at least 40ms after power rises above 2.7V
   * before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
   */
  display->hal.delay_microseconds(50000);

  /* Now we pull both RS and R/W low to begin commands */
  display->hal.pins.pin_write(&display->rs_pin, HD44780_PIN_LOW);
  display->hal.pins.pin_write(&display->enable_pin, HD44780_PIN_LOW);

  if (display->rw_pin.gpio != NULL)
    display->hal.pins.pin_write(&display->rw_pin, HD44780_PIN_LOW);

  return HD44780_OK;
}

void hd44780_command(HD44780 *display, uint8_t value)
{
  assert_param(display != NULL);
  hd44780_send(display, value, RESET);
}

void hd44780_send(HD44780 *display, uint8_t value, uint8_t mode)
{
  assert_param(display != NULL);
  assert_param(display->hal.pins.pin_write != NULL);

  display->hal.pins.pin_write(&display->rs_pin, mode);

  /* If there is a RW pin indicated, set it low to write */
  if (display->rw_pin.gpio != NULL)
    display->hal.pins.pin_write(&display->rw_pin, HD44780_PIN_LOW);

  if (display->displayfunction & LCD_8BITMODE)
    hd44780_write8bits(display, value);
  else
  {
    hd44780_write4bits(display, value >> 4);
    hd44780_write4bits(display, value);
  }
}

void hd44780_write(HD44780 *display, uint8_t value)
{
  assert_param(display != NULL);
  hd44780_send(display, value, SET);
}

void hd44780_write8bits(HD44780 *display, uint8_t value)
{
  assert_param(display != NULL);
  assert_param(display->hal.pins.pin_write != NULL);

  for (int i = 0; i < 8; ++i)
  {
    /* FIXME Use this if you implement reading which will use data pins as inputs:
             display->hal.pins.pin_config(&display->data_pins[i]) */
    display->hal.pins.pin_write(&display->data_pins[i], (value >> i) & 0x01);
  }

  hd44780_pulse_enable_pin(display);
}

void hd44780_write4bits(HD44780 *display, uint8_t value)
{
  assert_param(display != NULL);
  assert_param(display->hal.pins.pin_write != NULL);

  for (int i = 0; i < 4; ++i)
  {
    /* FIXME Use this if you implement reading which will use data pins as inputs:
             display->hal.pins.pin_config(&display->data_pins[i]) */
    display->hal.pins.pin_write(&display->data_pins[i], (value >> i) & 0x01);
  }

  hd44780_pulse_enable_pin(display);
}

void hd44780_pulse_enable_pin(HD44780 *display)
{
  assert_param(display != NULL);
  assert_param(display->hal.pins.pin_write != NULL);

  display->hal.pins.pin_write(&display->enable_pin, HD44780_PIN_LOW);
  display->hal.delay_microseconds(1);
  display->hal.pins.pin_write(&display->enable_pin, HD44780_PIN_HIGH);
  display->hal.delay_microseconds(1); // enable pulse must be >450ns
  display->hal.pins.pin_write(&display->enable_pin, HD44780_PIN_LOW);
  display->hal.delay_microseconds(100); // commands need > 37us to settle
}


/***** PinDriver for STM32F10X *****/
#ifdef HD44780_USE_STM32F10X

HD44780_Result stm32f10x_default_pin_config(HD44780_Pin *pin, HD44780_PinMode mode)
{
  assert_param(pin != NULL);
  assert_param(pin->gpio != NULL);

  if (pin == NULL || pin->gpio == NULL)
    return HD44780_ERROR;

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

    default: assert_param(0);
  }

  gpio_config.GPIO_Pin = pin->gpio_pin;
  GPIO_Init(pin->gpio, &gpio_config);

  return HD44780_OK;
}

HD44780_Result stm32f10x_default_pin_write(HD44780_Pin *pin, HD44780_PinState value)
{
  assert_param(pin != NULL);
  assert_param(pin->gpio != NULL);

  if (pin == NULL || pin->gpio == NULL)
    return HD44780_ERROR;

  GPIO_WriteBit(pin->gpio, pin->gpio_pin, (value == HD44780_PIN_LOW ? RESET : SET));

  return HD44780_OK;
}

/*
HD44780_PinState stm32f10x_default_pin_read(HD44780_Pin *pin)
{
  return HD44780_PIN_LOW;
}
*/

const PinDriver STM32F10X_LineDriver =
{
  stm32f10x_default_pin_config,
  stm32f10x_default_pin_write,
  // stm32f10x_default_pin_read
};

#endif
