#include <stdlib.h>
#include <stdint.h>
#include "hd44780.h"


#ifndef NDEBUG
  #define HD44780_ASSERT(x) \
    { \
      if (!(x)) \
      { \
        if (display->cfg.assert_failure_handler != NULL) \
          display->cfg.assert_failure_handler(__FILE__, __LINE__); \
      } \
    }

  #define HD44780_RETURN_ASSERT(x,ret) \
    do { \
      int condition = (x); \
      HD44780_ASSERT(condition) \
      if (!condition) \
        return (ret); \
    } while (0)
#else
  #define HD44780_ASSERT(x)
  #define HD44780_RETURN_ASSERT(x,ret)
#endif

#define HD44780_RETURN_IF_ERROR(result) \
  do {\
    HD44780_Result r = (result); \
    if (r != HD44780_RESULT_OK) \
      return r; \
  } while (0)


#define HD44780_CMD_CLEARDISPLAY 0x01
#define HD44780_CMD_RETURNHOME 0x02
#define HD44780_CMD_ENTRYMODESET 0x04
#define HD44780_CMD_DISPLAYCONTROL 0x08
#define HD44780_CMD_CURSORSHIFT 0x10
#define HD44780_CMD_FUNCTIONSET 0x20
#define HD44780_CMD_SETCGRAMADDR 0x40
#define HD44780_CMD_SETDDRAMADDR 0x80

// flags for display entry mode
#define HD44780_FLAG_ENTRYRIGHT 0x00
#define HD44780_FLAG_ENTRYLEFT 0x02
#define HD44780_FLAG_ENTRYSHIFTINCREMENT 0x01
#define HD44780_FLAG_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define HD44780_FLAG_DISPLAYON 0x04
#define HD44780_FLAG_DISPLAYOFF 0x00
#define HD44780_FLAG_CURSORON 0x02
#define HD44780_FLAG_CURSOROFF 0x00
#define HD44780_FLAG_BLINKON 0x01
#define HD44780_FLAG_BLINKOFF 0x00

// flags for display/cursor shift
#define HD44780_FLAG_DISPLAYMOVE 0x08
#define HD44780_FLAG_CURSORMOVE 0x00
#define HD44780_FLAG_MOVERIGHT 0x04
#define HD44780_FLAG_MOVELEFT 0x00

// flags for function set
#define HD44780_FLAG_8BITMODE 0x10
#define HD44780_FLAG_4BITMODE 0x00
#define HD44780_FLAG_2LINE 0x08
#define HD44780_FLAG_1LINE 0x00
#define HD44780_FLAG_5x10DOTS 0x04
#define HD44780_FLAG_5x8DOTS 0x00

const unsigned char russian[]={ 0x41, 0xA0, 0x42, 0xA1, 0xE0, 0x45,
0xA3, 0xA4, 0xA5,0xA6, 0x4B, 0xA7, 0x4D, 0x48, 0x4F, 0xA8, 0x50,0x43,
0x54, 0xA9, 0xAA, 0x58, 0xE1, 0xAB, 0xAC, 0xE2, 0xAD,0xAE, 0x62,
0xAF, 0xB0, 0xB1, 0x61, 0xB2, 0xB3, 0xB4, 0xE3, 0x65, 0xB6, 0xB7,
0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0x6F, 0xBE, 0x70, 0x63,0xBF,
0x79, 0xE4, 0x78, 0xE5, 0xC0, 0xC1, 0xE6, 0xC2, 0xC3, 0xC4, 0xC5,
0xC6, 0xC7 };

HD44780_Result hd44780_config(HD44780 *display);
HD44780_Result hd44780_command(HD44780 *display, uint8_t value);
HD44780_Result hd44780_write_byte(HD44780 *display, uint8_t value);
HD44780_Result hd44780_send(HD44780 *display, uint8_t value, HD44780_PinState rs_mode);
HD44780_Result hd44780_write_bits(HD44780 *display, uint8_t value);
HD44780_Result hd44780_read_bits(HD44780 *display, uint8_t *value);
HD44780_Result hd44780_pulse_enable_pin(HD44780 *display);

HD44780_Result hd44780_init(HD44780 *display, HD44780_Mode mode,
  const HD44780_Config *config, uint8_t columns, uint8_t rows, HD44780_CharSize charsize)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(config != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(config->gpios != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(config->gpios->write != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(config->delay_microseconds != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(columns > 0, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(rows > 0, HD44780_RESULT_ERROR);

  display->cfg = *config;

  HD44780_GPIO_Interface *const gpios = display->cfg.gpios;
  HD44780_DelayMicrosecondsFn delay_microseconds = display->cfg.delay_microseconds;

  if (gpios->configure != NULL)
  {
    HD44780_RETURN_IF_ERROR(gpios->configure(gpios, HD44780_PIN_RS, HD44780_PINMODE_OUTPUT));
    HD44780_RETURN_IF_ERROR(gpios->configure(gpios, HD44780_PIN_ENABLE, HD44780_PINMODE_OUTPUT));

    if (display->cfg.options & HD44780_OPT_USE_RW)
      HD44780_RETURN_IF_ERROR(gpios->configure(gpios, HD44780_PIN_RW, HD44780_PINMODE_OUTPUT));

    if (display->cfg.options & HD44780_OPT_USE_BACKLIGHT)
      HD44780_RETURN_IF_ERROR(gpios->configure(gpios, HD44780_PIN_BACKLIGHT, HD44780_PINMODE_OUTPUT));
  }

  if (display->cfg.options & HD44780_OPT_USE_BACKLIGHT)
    HD44780_RETURN_IF_ERROR(gpios->write(gpios, HD44780_PIN_BACKLIGHT, HD44780_PINSTATE_LOW));

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
  if (charsize == HD44780_CHARSIZE_5x10  && rows == 1)
    display->displayfunction |= HD44780_FLAG_5x10DOTS;

  if (rows > 1)
    display->displayfunction |= HD44780_FLAG_2LINE;

  display->columns_amount = columns;
  display->lines_amount = rows;
  display->currline = 0;

  if (hd44780_config(display) != HD44780_RESULT_OK)
    return HD44780_RESULT_ERROR;

  /* Put the LCD into 4 bit or 8 bit mode */
  if (display->displayfunction & HD44780_FLAG_8BITMODE)
  {
    /*** This is according to the hitachi HD44780 datasheet page 45 figure 23 ***/

    /* Send function set command sequence */
    HD44780_RETURN_IF_ERROR(hd44780_command(display, HD44780_CMD_FUNCTIONSET | display->displayfunction));
    delay_microseconds(4500);  // wait more than 4.1ms

    /* Second try */
    HD44780_RETURN_IF_ERROR(hd44780_command(display, HD44780_CMD_FUNCTIONSET | display->displayfunction));
    delay_microseconds(150);

    /* Third go */
    HD44780_RETURN_IF_ERROR(hd44780_command(display, HD44780_CMD_FUNCTIONSET | display->displayfunction));
  }
  else
  {
    /*** This is according to the hitachi HD44780 datasheet figure 24, pg 46 ***/

    /* We start in 8bit mode, try to set 4 bit mode */
    HD44780_RETURN_IF_ERROR(hd44780_write_bits(display, 0x03));
    delay_microseconds(4500); // wait min 4.1ms

    /* Second try */
    HD44780_RETURN_IF_ERROR(hd44780_write_bits(display, 0x03));
    delay_microseconds(4500); // wait min 4.1ms

    /* Third go! */
    HD44780_RETURN_IF_ERROR(hd44780_write_bits(display, 0x03));
    delay_microseconds(150);

    /* Finally, set to 4-bit interface */
    HD44780_RETURN_IF_ERROR(hd44780_write_bits(display, 0x02));
  }

  /* Finally, set # lines, font size, etc. */
  HD44780_RETURN_IF_ERROR(hd44780_command(display, HD44780_CMD_FUNCTIONSET | display->displayfunction));

  /* Turn the display on with no cursor or blinking default */
  display->displaycontrol = HD44780_FLAG_DISPLAYON | HD44780_FLAG_CURSOROFF | HD44780_FLAG_BLINKOFF;
  HD44780_RETURN_IF_ERROR(hd44780_display_on(display));

  /* Clear it off */
  HD44780_RETURN_IF_ERROR(hd44780_clear(display));
  /* Initialize to default text direction (for romance languages) */
  display->displaymode = HD44780_FLAG_ENTRYLEFT | HD44780_FLAG_ENTRYSHIFTDECREMENT;
  /* Set the entry mode */
  HD44780_RETURN_IF_ERROR(hd44780_command(display, HD44780_CMD_ENTRYMODESET | display->displaymode));

  return HD44780_RESULT_OK;
}

HD44780_Result hd44780_write_char(HD44780 *display, char c)
{
  if(c >= 192)
    return hd44780_write_byte(display, (uint8_t)russian[c-192]);
  else
    return hd44780_write_byte(display, (uint8_t)c);
}

HD44780_Result hd44780_write_string(HD44780 *display, const char *s)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(s != NULL, HD44780_RESULT_ERROR);

  while (*s)
    HD44780_RETURN_IF_ERROR(hd44780_write_char(display, *s++));

  return HD44780_RESULT_OK;
}

HD44780_Result hd44780_clear(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.delay_microseconds != NULL, HD44780_RESULT_ERROR);

  HD44780_RETURN_IF_ERROR(hd44780_command(display, HD44780_CMD_CLEARDISPLAY)); // clear display, set cursor position to zero
  display->cfg.delay_microseconds(3000); // this command takes a long time!

  return HD44780_RESULT_OK;
}

HD44780_Result hd44780_home(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.delay_microseconds != NULL, HD44780_RESULT_ERROR);

  HD44780_RETURN_IF_ERROR(hd44780_command(display, HD44780_CMD_RETURNHOME));
  display->cfg.delay_microseconds(3000);  // this command takes a long time!

  return HD44780_RESULT_OK;
}

HD44780_Result hd44780_scroll_left(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  return hd44780_command(display,
      HD44780_CMD_CURSORSHIFT | HD44780_FLAG_DISPLAYMOVE | HD44780_FLAG_MOVELEFT);
}

HD44780_Result hd44780_scroll_right(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  return hd44780_command(display,
      HD44780_CMD_CURSORSHIFT | HD44780_FLAG_DISPLAYMOVE | HD44780_FLAG_MOVERIGHT);
}

HD44780_Result hd44780_left_to_right(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);

  display->displaymode |= HD44780_FLAG_ENTRYLEFT;
  return hd44780_command(display, HD44780_CMD_ENTRYMODESET | display->displaymode);
}

HD44780_Result hd44780_right_to_left(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);

  display->displaymode &= ~HD44780_FLAG_ENTRYLEFT;
  return hd44780_command(display, HD44780_CMD_ENTRYMODESET | display->displaymode);
}

/* FIXME moves the cursor out of screen */
HD44780_Result hd44780_create_char(HD44780 *display, uint8_t location, const uint8_t *charmap)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(charmap != NULL, HD44780_RESULT_ERROR);

  location &= 0x7; // we only have 8 locations 0-7
  HD44780_RETURN_IF_ERROR(hd44780_command(display, HD44780_CMD_SETCGRAMADDR | (location << 3)));

  for (unsigned i = 0; i < 8; ++i)
    HD44780_RETURN_IF_ERROR(hd44780_write_byte(display, charmap[i]));

  return HD44780_RESULT_OK;
}

HD44780_Result hd44780_move_cursor(HD44780 *display, uint8_t column, uint8_t row)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);

  static const int row_offsets[] = { 0x00, 0x40, 0x10, 0x50 };

  if (row > display->lines_amount)
    row = display->lines_amount - 1; // we count rows starting with zero

  return hd44780_command(display, HD44780_CMD_SETDDRAMADDR | (column + row_offsets[row]));
}

HD44780_Result hd44780_display_on(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);

  display->displaycontrol |= HD44780_FLAG_DISPLAYON;
  return hd44780_command(display, HD44780_CMD_DISPLAYCONTROL | display->displaycontrol);
}

HD44780_Result hd44780_display_off(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);

  display->displaycontrol &= ~HD44780_FLAG_DISPLAYON;
  return hd44780_command(display, HD44780_CMD_DISPLAYCONTROL | display->displaycontrol);
}

HD44780_Result hd44780_blink_on(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);

  display->displaycontrol |= HD44780_FLAG_BLINKON;
  return hd44780_command(display, HD44780_CMD_DISPLAYCONTROL | display->displaycontrol);
}

HD44780_Result hd44780_blink_off(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);

  display->displaycontrol &= ~HD44780_FLAG_BLINKON;
  return hd44780_command(display, HD44780_CMD_DISPLAYCONTROL | display->displaycontrol);
}

HD44780_Result hd44780_cursor_on(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);

  display->displaycontrol |= HD44780_FLAG_CURSORON;
  return hd44780_command(display, HD44780_CMD_DISPLAYCONTROL | display->displaycontrol);
}

HD44780_Result hd44780_cursor_off(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);

  display->displaycontrol &= ~HD44780_FLAG_CURSORON;
  return hd44780_command(display, HD44780_CMD_DISPLAYCONTROL | display->displaycontrol);
}

HD44780_Result hd44780_autoscroll_on(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);

  display->displaymode |= HD44780_FLAG_ENTRYSHIFTINCREMENT;
  return hd44780_command(display, HD44780_CMD_ENTRYMODESET | display->displaymode);
}

HD44780_Result hd44780_autoscroll_off(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);

  display->displaymode &= ~HD44780_FLAG_ENTRYSHIFTINCREMENT;
  return hd44780_command(display, HD44780_CMD_ENTRYMODESET | display->displaymode);
}

HD44780_Result hd44780_backlight_on(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios->write != NULL, HD44780_RESULT_ERROR);

  return display->cfg.gpios->write(display->cfg.gpios, HD44780_PIN_BACKLIGHT, HD44780_PINSTATE_HIGH);
}

HD44780_Result hd44780_backlight_off(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios->write != NULL, HD44780_RESULT_ERROR);

  return display->cfg.gpios->write(display->cfg.gpios, HD44780_PIN_BACKLIGHT, HD44780_PINSTATE_LOW);
}

/*----------------------*/
/*     Internal API     */
/*----------------------*/

HD44780_Result hd44780_config(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.delay_microseconds != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios->write != NULL, HD44780_RESULT_ERROR);

  HD44780_GPIO_Interface *const gpios = display->cfg.gpios;

  for (unsigned i = 0; i < display->dp_amount; ++i)
  {
    if (gpios->configure != NULL)
      HD44780_RETURN_IF_ERROR(gpios->configure(gpios, display->dp_first + i, HD44780_PINMODE_OUTPUT));

    HD44780_RETURN_IF_ERROR(gpios->write(gpios, display->dp_first + i, HD44780_PINSTATE_LOW));
  }

  /* SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
   * according to datasheet, we need at least 40ms after power rises above 2.7V
   * before sending commands.
   */
  display->cfg.delay_microseconds(50000);

  /* Now we pull both RS and R/W low to begin commands */
  HD44780_RETURN_IF_ERROR(gpios->write(gpios, HD44780_PIN_RS, HD44780_PINSTATE_LOW));
  HD44780_RETURN_IF_ERROR(gpios->write(gpios, HD44780_PIN_ENABLE, HD44780_PINSTATE_LOW));

  if (display->cfg.options & HD44780_OPT_USE_RW)
    HD44780_RETURN_IF_ERROR(gpios->write(gpios, HD44780_PIN_RW, HD44780_PINSTATE_LOW));

  return HD44780_RESULT_OK;
}

HD44780_Result hd44780_command(HD44780 *display, uint8_t value)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  return hd44780_send(display, value, HD44780_PINSTATE_LOW);
}

HD44780_Result hd44780_write_byte(HD44780 *display, uint8_t value)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  return hd44780_send(display, value, HD44780_PINSTATE_HIGH);
}

HD44780_Result hd44780_send(HD44780 *display, uint8_t value, HD44780_PinState rs_mode)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios->write != NULL, HD44780_RESULT_ERROR);

  HD44780_GPIO_Interface *const gpios = display->cfg.gpios;

  HD44780_RETURN_IF_ERROR(gpios->write(gpios, HD44780_PIN_RS, rs_mode));

  if (display->cfg.options & HD44780_OPT_USE_RW)
    HD44780_RETURN_IF_ERROR(gpios->write(gpios, HD44780_PIN_RW, HD44780_PINSTATE_LOW));

  if (display->displayfunction & HD44780_FLAG_8BITMODE)
    HD44780_RETURN_IF_ERROR(hd44780_write_bits(display, value));
  else
  {
    HD44780_RETURN_IF_ERROR(hd44780_write_bits(display, value >> 4));
    HD44780_RETURN_IF_ERROR(hd44780_write_bits(display, value));
  }

  return HD44780_RESULT_OK;
}

HD44780_Result hd44780_write_bits(HD44780 *display, uint8_t value)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios->configure != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios->write != NULL, HD44780_RESULT_ERROR);

  HD44780_GPIO_Interface *const gpios = display->cfg.gpios;

  for (unsigned i = 0; i < display->dp_amount; ++i)
  {
    HD44780_RETURN_IF_ERROR(gpios->configure(gpios, display->dp_first + i, HD44780_PINMODE_OUTPUT));
    HD44780_RETURN_IF_ERROR(gpios->write(gpios, display->dp_first + i, (value >> i) & 0x01));
  }

  HD44780_RETURN_IF_ERROR(hd44780_pulse_enable_pin(display));

  return HD44780_RESULT_OK;
}

HD44780_Result hd44780_read_bits(HD44780 *display, uint8_t *value)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios->configure != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios->write != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(value != NULL, HD44780_RESULT_ERROR);

  HD44780_GPIO_Interface *const gpios = display->cfg.gpios;

  if (display->cfg.options & HD44780_OPT_USE_RW)
    HD44780_RETURN_IF_ERROR(gpios->write(gpios, HD44780_PIN_RW, HD44780_PINSTATE_LOW));

  uint8_t value_read = 0;
  uint8_t bit = 0;

  for (unsigned i = 0; i < display->dp_amount; ++i)
  {
    HD44780_RETURN_IF_ERROR(gpios->configure(gpios, display->dp_first + i, HD44780_PINMODE_INPUT));
    HD44780_RETURN_IF_ERROR(gpios->read(gpios, display->dp_first + i, &bit));
    value_read = (value_read << i) | (bit & 0x01);
  }

  HD44780_RETURN_IF_ERROR(hd44780_pulse_enable_pin(display));
  *value = value_read;

  return HD44780_RESULT_OK;
}

HD44780_Result hd44780_pulse_enable_pin(HD44780 *display)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.gpios->write != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(display->cfg.delay_microseconds != NULL, HD44780_RESULT_ERROR);

  HD44780_GPIO_Interface *const gpios = display->cfg.gpios;
  HD44780_DelayMicrosecondsFn delay_microseconds = display->cfg.delay_microseconds;

  HD44780_RETURN_IF_ERROR(gpios->write(gpios, HD44780_PIN_ENABLE, HD44780_PINSTATE_LOW));
  delay_microseconds(1);

  HD44780_RETURN_IF_ERROR(gpios->write(gpios, HD44780_PIN_ENABLE, HD44780_PINSTATE_HIGH));
  delay_microseconds(1); // enable pulse must be >450ns

  HD44780_RETURN_IF_ERROR(gpios->write(gpios, HD44780_PIN_ENABLE, HD44780_PINSTATE_LOW));
  delay_microseconds(100); // commands need > 37us to settle

  return HD44780_RESULT_OK;
}
