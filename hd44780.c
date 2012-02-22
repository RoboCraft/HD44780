#include <stdlib.h>
#include <stdint.h>
#include "hd44780.h"


void HD44780_HANG_ASSERT(int x)
{
  if (!x)
  {
    volatile int i;
    do ++i; while (1);
  }
}

#ifndef NDEBUG
  #define HD44780_RETURN_ASSERT(x,ret) \
    HD44780_HANG_ASSERT(x)
#else
  #define HD44780_RETURN_ASSERT(x,ret) \
    do { if (!(x)) return (ret); while (1); } while (0)
#endif


HD44780_Result hd44780_init(HD44780 *display, HD44780_Mode mode,
    const HD44780_HAL *hal, uint8_t cols, uint8_t lines, uint8_t charsize)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(hal != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(hal->pin_driver != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(hal->pin_driver->write != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(hal->delay_microseconds != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(cols > 0, HD44780_ERROR);
  HD44780_RETURN_ASSERT(lines > 0, HD44780_ERROR);

  display->hal = *hal;

  if (display->hal.pin_driver->configure != NULL)
  {
    display->hal.pin_driver->configure(display->hal.pin_driver, HD44780_PIN_RS, HD44780_PIN_OUTPUT);
    display->hal.pin_driver->configure(display->hal.pin_driver, HD44780_PIN_ENABLE, HD44780_PIN_OUTPUT);
    display->hal.pin_driver->configure(display->hal.pin_driver, HD44780_PIN_RW, HD44780_PIN_OUTPUT);
    display->hal.pin_driver->configure(display->hal.pin_driver, HD44780_PIN_BACKLIGHT, HD44780_PIN_OUTPUT);
  }

  display->hal.pin_driver->write(display->hal.pin_driver, HD44780_PIN_BACKLIGHT, HD44780_PIN_LOW);

  if (mode == HD44780_MODE_4BIT)
  {
    display->displayfunction = HD44780_FLAG_4BITMODE | HD44780_FLAG_1LINE | HD44780_FLAG_5x8DOTS;
    display->dp_first = HD44780_PIN_DP4;
    display->dp_amount = 4;
  }
  else
  {
    display->displayfunction = HD44780_FLAG_8BITMODE | HD44780_FLAG_1LINE | HD44780_FLAG_5x8DOTS;
    display->dp_first = HD44780_PIN_DP0;
    display->dp_amount = 8;
  }

  /* For some 1 line displays you can select a 10 pixel high font */
  if ((charsize != 0) && (lines == 1))
    display->displayfunction |= HD44780_FLAG_5x10DOTS;

  if (lines > 1)
    display->displayfunction |= HD44780_FLAG_2LINE;

  display->columns_amount = cols;
  display->lines_amount = lines;
  display->currline = 0;

  if (hd44780_config(display) != HD44780_OK)
    return HD44780_ERROR;

  /* Put the LCD into 4 bit or 8 bit mode */
  if (! (display->displayfunction & HD44780_FLAG_8BITMODE))
  {
    /* This is according to the hitachi HD44780 datasheet figure 24, pg 46 */

    /* We start in 8bit mode, try to set 4 bit mode */
    hd44780_write_bits(display, 0x03);
    display->hal.delay_microseconds(4500); // wait min 4.1ms

    /* Second try */
    hd44780_write_bits(display, 0x03);
    display->hal.delay_microseconds(4500); // wait min 4.1ms

    /* Third go! */
    hd44780_write_bits(display, 0x03);
    display->hal.delay_microseconds(150);

    /* Finally, set to 4-bit interface */
    hd44780_write_bits(display, 0x02);
  }
  else
  {
    /* This is according to the hitachi HD44780 datasheet page 45 figure 23 */

    /* Send function set command sequence */
    hd44780_command(display, HD44780_CMD_FUNCTIONSET | display->displayfunction);
    display->hal.delay_microseconds(4500);  // wait more than 4.1ms

    /* Second try */
    hd44780_command(display, HD44780_CMD_FUNCTIONSET | display->displayfunction);
    display->hal.delay_microseconds(150);

    /* Third go */
    hd44780_command(display, HD44780_CMD_FUNCTIONSET | display->displayfunction);
  }

  /* Finally, set # lines, font size, etc. */
  hd44780_command(display, HD44780_CMD_FUNCTIONSET | display->displayfunction);

  /* Turn the display on with no cursor or blinking default */
  display->displaycontrol = HD44780_FLAG_DISPLAYON | HD44780_FLAG_CURSOROFF | HD44780_FLAG_BLINKOFF;
  hd44780_display_on(display);

  /* Clear it off */
  hd44780_clear(display);

  /* Initialize to default text direction (for romance languages) */
  display->displaymode = HD44780_FLAG_ENTRYLEFT | HD44780_FLAG_ENTRYSHIFTDECREMENT;

  /* Set the entry mode */
  hd44780_command(display, HD44780_CMD_ENTRYMODESET | display->displaymode);

  return HD44780_OK;
}

HD44780_Result hd44780_write_byte(HD44780 *display, uint8_t value)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  hd44780_send(display, value, HD44780_PIN_HIGH);

  return HD44780_OK;
}

HD44780_Result hd44780_write_char(HD44780 *display, char c)
{
  return hd44780_write_byte(display, (uint8_t)c);
}

HD44780_Result hd44780_write_string(HD44780 *display, const char *s)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(s != NULL, HD44780_ERROR);

  while (*s)
    hd44780_write_char(display, *s++);

  return HD44780_OK;
}

HD44780_Result hd44780_clear(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.delay_microseconds != NULL, HD44780_ERROR);

  hd44780_command(display, HD44780_CMD_CLEARDISPLAY); // clear display, set cursor position to zero
  display->hal.delay_microseconds(3000); // this command takes a long time!

  return HD44780_OK;
}

HD44780_Result hd44780_home(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.delay_microseconds != NULL, HD44780_ERROR);

  hd44780_command(display, HD44780_CMD_RETURNHOME);  // set cursor position to zero
  display->hal.delay_microseconds(3000);  // this command takes a long time!

  return HD44780_OK;
}

HD44780_Result hd44780_display_on(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  display->displaycontrol |= HD44780_FLAG_DISPLAYON;
  return hd44780_command(display, HD44780_CMD_DISPLAYCONTROL | display->displaycontrol);
}

HD44780_Result hd44780_display_off(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  display->displaycontrol &= ~HD44780_FLAG_DISPLAYON;
  return hd44780_command(display, HD44780_CMD_DISPLAYCONTROL | display->displaycontrol);
}

HD44780_Result hd44780_blink_on(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  display->displaycontrol |= HD44780_FLAG_BLINKON;
  return hd44780_command(display, HD44780_CMD_DISPLAYCONTROL | display->displaycontrol);
}

HD44780_Result hd44780_blink_off(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  display->displaycontrol &= ~HD44780_FLAG_BLINKON;
  return hd44780_command(display, HD44780_CMD_DISPLAYCONTROL | display->displaycontrol);
}

HD44780_Result hd44780_cursor_on(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  display->displaycontrol |= HD44780_FLAG_CURSORON;
  return hd44780_command(display, HD44780_CMD_DISPLAYCONTROL | display->displaycontrol);
}

HD44780_Result hd44780_cursor_off(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  display->displaycontrol &= ~HD44780_FLAG_CURSORON;
  return hd44780_command(display, HD44780_CMD_DISPLAYCONTROL | display->displaycontrol);
}

HD44780_Result hd44780_autoscroll_on(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  display->displaymode |= HD44780_FLAG_ENTRYSHIFTINCREMENT;
  return hd44780_command(display, HD44780_CMD_ENTRYMODESET | display->displaymode);
}

HD44780_Result hd44780_autoscroll_off(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  display->displaymode &= ~HD44780_FLAG_ENTRYSHIFTINCREMENT;
  return hd44780_command(display, HD44780_CMD_ENTRYMODESET | display->displaymode);
}

HD44780_Result hd44780_scroll_left(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  return hd44780_command(display,
      HD44780_CMD_CURSORSHIFT | HD44780_FLAG_DISPLAYMOVE | HD44780_FLAG_MOVELEFT);
}

HD44780_Result hd44780_scroll_right(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  return hd44780_command(display,
      HD44780_CMD_CURSORSHIFT | HD44780_FLAG_DISPLAYMOVE | HD44780_FLAG_MOVERIGHT);
}

HD44780_Result hd44780_left_to_right(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  display->displaymode |= HD44780_FLAG_ENTRYLEFT;
  return hd44780_command(display, HD44780_CMD_ENTRYMODESET | display->displaymode);
}

HD44780_Result hd44780_right_to_left(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  display->displaymode &= ~HD44780_FLAG_ENTRYLEFT;
  return hd44780_command(display, HD44780_CMD_ENTRYMODESET | display->displaymode);
}

/* FIXME moves the cursor out of screen */
HD44780_Result hd44780_create_char(HD44780 *display, uint8_t location, const uint8_t *charmap)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(charmap != NULL, HD44780_ERROR);

  location &= 0x7; // we only have 8 locations 0-7

  if (hd44780_command(display, HD44780_CMD_SETCGRAMADDR | (location << 3)) != HD44780_OK)
    return HD44780_ERROR;

  for (unsigned i = 0; i < 8; ++i)
  {
    if (hd44780_write_byte(display, charmap[i]) != HD44780_OK)
      return HD44780_ERROR;
  }

  return HD44780_OK;
}

HD44780_Result hd44780_move_cursor(HD44780 *display, uint8_t column, uint8_t row)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  static const int row_offsets[] = { 0x00, 0x40, 0x10, 0x50 };

  if (row > display->lines_amount)
    row = display->lines_amount - 1; // we count rows starting with zero

  return hd44780_command(display, HD44780_CMD_SETDDRAMADDR | (column + row_offsets[row]));
}

HD44780_Result hd44780_config(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.delay_microseconds != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.pin_driver->write != NULL, HD44780_ERROR);

  for (unsigned i = 0; i < display->dp_amount; ++i)
  {
    if (display->hal.pin_driver->configure != NULL)
      display->hal.pin_driver->configure(display->hal.pin_driver, display->dp_first + i, HD44780_PIN_OUTPUT);

    display->hal.pin_driver->write(display->hal.pin_driver, display->dp_first + i, HD44780_PIN_LOW);
  }

  /* SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
   * according to datasheet, we need at least 40ms after power rises above 2.7V
   * before sending commands.
   */
  display->hal.delay_microseconds(50000);

  /* Now we pull both RS and R/W low to begin commands */
  display->hal.pin_driver->write(display->hal.pin_driver, HD44780_PIN_RS, HD44780_PIN_LOW);
  display->hal.pin_driver->write(display->hal.pin_driver, HD44780_PIN_ENABLE, HD44780_PIN_LOW);
  display->hal.pin_driver->write(display->hal.pin_driver, HD44780_PIN_RW, HD44780_PIN_LOW);

  return HD44780_OK;
}

HD44780_Result hd44780_command(HD44780 *display, uint8_t value)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);

  hd44780_send(display, value, HD44780_PIN_LOW);

  return HD44780_OK;
}

HD44780_Result hd44780_send(HD44780 *display, uint8_t value, HD44780_PinState rs_mode)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.pin_driver->write != NULL, HD44780_ERROR);

  display->hal.pin_driver->write(display->hal.pin_driver, HD44780_PIN_RS, rs_mode);
  display->hal.pin_driver->write(display->hal.pin_driver, HD44780_PIN_RW, HD44780_PIN_LOW);

  if (display->displayfunction & HD44780_FLAG_8BITMODE)
    hd44780_write_bits(display, value);
  else
  {
    hd44780_write_bits(display, value >> 4);
    hd44780_write_bits(display, value);
  }

  return HD44780_OK;
}

HD44780_Result hd44780_write_bits(HD44780 *display, uint8_t value)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.pin_driver->configure != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.pin_driver->write != NULL, HD44780_ERROR);

  for (unsigned i = 0; i < display->dp_amount; ++i)
  {
    display->hal.pin_driver->configure(display->hal.pin_driver, display->dp_first + i, HD44780_PIN_OUTPUT);
    display->hal.pin_driver->write(display->hal.pin_driver, display->dp_first + i, (value >> i) & 0x01);
  }

  hd44780_pulse_enable_pin(display);

  return HD44780_OK;
}

HD44780_Result hd44780_read_bits(HD44780 *display, uint8_t *value)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.pin_driver->configure != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.pin_driver->write != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(value != NULL, HD44780_ERROR);

  uint8_t value_read = 0;
  uint8_t bit = 0;

  for (unsigned i = 0; i < display->dp_amount; ++i)
  {
    display->hal.pin_driver->configure(display->hal.pin_driver, display->dp_first + i, HD44780_PIN_INPUT);
    display->hal.pin_driver->read(display->hal.pin_driver, display->dp_first + i, &bit);
    value_read = (value_read << i) | (bit & 0x01);
  }

  hd44780_pulse_enable_pin(display);
  *value = value_read;

  return HD44780_OK;
}

HD44780_Result hd44780_pulse_enable_pin(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.pin_driver->write != NULL, HD44780_ERROR);
  HD44780_RETURN_ASSERT(display->hal.delay_microseconds != NULL, HD44780_ERROR);

  display->hal.pin_driver->write(display->hal.pin_driver, HD44780_PIN_ENABLE, HD44780_PIN_LOW);
  display->hal.delay_microseconds(1);
  display->hal.pin_driver->write(display->hal.pin_driver, HD44780_PIN_ENABLE, HD44780_PIN_HIGH);
  display->hal.delay_microseconds(1); // enable pulse must be >450ns
  display->hal.pin_driver->write(display->hal.pin_driver, HD44780_PIN_ENABLE, HD44780_PIN_LOW);
  display->hal.delay_microseconds(100); // commands need > 37us to settle

  return HD44780_OK;
}
