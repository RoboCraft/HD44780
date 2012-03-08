#include <stdlib.h>
#include <stdint.h>
#include "hd44780.h"

HD44780_Result hd44780_init(HD44780 *display, HD44780_Mode mode,
  const HD44780_Config *config, uint8_t cols, uint8_t lines, uint8_t charsize)
{
  HD44780_RETURN_ASSERT(display != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(config != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(config->gpios != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(config->gpios->write != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(config->delay_microseconds != NULL, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(cols > 0, HD44780_RESULT_ERROR);
  HD44780_RETURN_ASSERT(lines > 0, HD44780_RESULT_ERROR);

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
  if ((charsize != 0) && (lines == 1))
    display->displayfunction |= HD44780_FLAG_5x10DOTS;

  if (lines > 1)
    display->displayfunction |= HD44780_FLAG_2LINE;

  display->columns_amount = cols;
  display->lines_amount = lines;
  display->currline = 0;

  if (hd44780_config(display) != HD44780_RESULT_OK)
    return HD44780_RESULT_ERROR;

  /* Put the LCD into 4 bit or 8 bit mode */
  if (! (display->displayfunction & HD44780_FLAG_8BITMODE))
  {
    /* This is according to the hitachi HD44780 datasheet figure 24, pg 46 */

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
  else
  {
    /* This is according to the hitachi HD44780 datasheet page 45 figure 23 */

    /* Send function set command sequence */
    HD44780_RETURN_IF_ERROR(hd44780_command(display, HD44780_CMD_FUNCTIONSET | display->displayfunction));
    delay_microseconds(4500);  // wait more than 4.1ms

    /* Second try */
    HD44780_RETURN_IF_ERROR(hd44780_command(display, HD44780_CMD_FUNCTIONSET | display->displayfunction));
    delay_microseconds(150);

    /* Third go */
    HD44780_RETURN_IF_ERROR(hd44780_command(display, HD44780_CMD_FUNCTIONSET | display->displayfunction));
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
