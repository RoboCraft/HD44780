#ifndef HC44780_H_
#define HC44780_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HD44780_USE_STM32F10X
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#endif

#include <stdint.h>

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

/* STM32 pin definition for HD44780 */
typedef struct
{
  GPIO_TypeDef *gpio;
  uint16_t gpio_pin;
} HD44780_Pin;

/* HD44780 pinout description structure */
typedef struct
{
  HD44780_Pin rs;         // LOW: command.  HIGH: character.
  HD44780_Pin en;     // activated by a HIGH pulse.
  HD44780_Pin rw;         // LOW: write to LCD.  HIGH: read from LCD.
  HD44780_Pin backlight;  // should be connected to base/gate of transistor/FET
  HD44780_Pin dp[8];      // data pins
} HD44780_Pinout;

typedef enum { HD44780_OK, HD44780_ERROR } HD44780_Result;
typedef enum { HD44780_PIN_INPUT, HD44780_PIN_OUTPUT } HD44780_PinMode;
typedef enum { HD44780_PIN_LOW, HD44780_PIN_HIGH } HD44780_PinState;

/* Hardware-dependent pin control interface */
typedef struct
{
  HD44780_Result (*configure)(HD44780_Pin *pin, HD44780_PinMode mode);
  HD44780_Result (*write)(HD44780_Pin *pin, HD44780_PinState value);
} PinDriver;

#ifdef HD44780_USE_STM32F10X
extern const PinDriver STM32F10X_PinDriver;
#endif

/* Hardware abstraction layer */
typedef struct
{
  PinDriver pins;
  void (*delay_microseconds)(uint32_t us);
} HD44780_HAL;

/* HD44780 control structure */
typedef struct
{
  HD44780_HAL hal;

  uint8_t displayfunction;
  uint8_t displaycontrol;
  uint8_t displaymode;
  uint8_t initialized;
  uint8_t numlines;
  uint8_t currline;

  HD44780_Pinout pinout;
  unsigned dp_offset;
  unsigned dp_amount;
} HD44780;

typedef enum { HD44780_MODE_4BIT, HD44780_MODE_8BIT } HD44780_Mode;

/***** User API *****/

HD44780_Result hd44780_init(HD44780 *display, HD44780_Mode mode,
    const HD44780_HAL *hal, const HD44780_Pinout *pinout,
    uint8_t cols, uint8_t lines, uint8_t charsize);
HD44780_Result hd44780_write_byte(HD44780 *display, uint8_t value);
HD44780_Result hd44780_write_char(HD44780 *display, char c);
HD44780_Result hd44780_write_string(HD44780 *display, const char *s);
HD44780_Result hd44780_clear(HD44780 *display);
HD44780_Result hd44780_home(HD44780 *display);
HD44780_Result hd44780_scroll_left(HD44780 *display);
HD44780_Result hd44780_scroll_right(HD44780 *display);
HD44780_Result hd44780_left_to_right(HD44780 *display);
HD44780_Result hd44780_right_to_left(HD44780 *display);
HD44780_Result hd44780_create_char(HD44780 *display, uint8_t location, const uint8_t *charmap);
HD44780_Result hd44780_move_cursor(HD44780 *display, uint8_t col, uint8_t line);

HD44780_Result hd44780_display_on(HD44780 *display);
HD44780_Result hd44780_display_off(HD44780 *display);
HD44780_Result hd44780_blink_on(HD44780 *display);
HD44780_Result hd44780_blink_off(HD44780 *display);
HD44780_Result hd44780_cursor_on(HD44780 *display);
HD44780_Result hd44780_cursor_off(HD44780 *display);
HD44780_Result hd44780_autoscroll_on(HD44780 *display);
HD44780_Result hd44780_autoscroll_off(HD44780 *display);

/***** Low-level API *****/

HD44780_Result hd44780_config(HD44780 *display);
HD44780_Result hd44780_command(HD44780 *display, uint8_t value);
HD44780_Result hd44780_send(HD44780 *display, uint8_t value, uint8_t mode);
HD44780_Result hd44780_write8bits(HD44780 *display, uint8_t value);
HD44780_Result hd44780_write4bits(HD44780 *display, uint8_t value);
HD44780_Result hd44780_pulse_enable_pin(HD44780 *display);

#define HD44780_MAKE_8BITS(b0,b1,b2,b3,b4,b5,b6,b7) \
    (((b0) & 1) | \
     ((b1) & 1) << 1 | \
     ((b2) & 1) << 2 | \
     ((b3) & 1) << 3 | \
     ((b4) & 1) << 4 | \
     ((b5) & 1) << 5 | \
     ((b6) & 1) << 6 | \
     ((b7) & 1) << 7)

#ifdef __cplusplus
}
#endif

#endif /* HC44780_H_ */
