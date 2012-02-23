#ifndef HC44780_H_
#define HC44780_H_

#ifdef __cplusplus
extern "C" {
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

typedef enum { HD44780_OK, HD44780_ERROR } HD44780_Result;
typedef enum { HD44780_PIN_INPUT, HD44780_PIN_OUTPUT } HD44780_PinMode;
typedef enum { HD44780_PIN_LOW, HD44780_PIN_HIGH } HD44780_PinState;

/* Abstract HD44780 pin definition */
typedef enum
{
  HD44780_PIN_RS,        // LOW: command.  HIGH: character.
  HD44780_PIN_ENABLE,    // latch, activated by a HIGH pulse.
  HD44780_PIN_RW,        // optional; LOW: write to LCD, HIGH: read from LCD
  HD44780_PIN_BACKLIGHT, // optional; should be connected to base/gate of transistor/FET

  /* Data pins DP0..DP7; in 4-bit mode DP0..DP3 are not needed. */
  HD44780_PIN_DP0,
  HD44780_PIN_DP1,
  HD44780_PIN_DP2,
  HD44780_PIN_DP3,
  HD44780_PIN_DP4,
  HD44780_PIN_DP5,
  HD44780_PIN_DP6,
  HD44780_PIN_DP7,

  HD44780_PIN_AMOUNT // enum member counter, must be last
} HD44780_Pin;

/* Hardware-dependent pin control interface.
 * configure() function is optional if you want to configure
 * the display pins manually.
 */
struct HD44780_GPIO_Interface_Struct;
typedef struct HD44780_GPIO_Interface_Struct HD44780_GPIO_Interface;

struct HD44780_GPIO_Interface_Struct
{
  HD44780_Result (*configure)(HD44780_GPIO_Interface *driver, HD44780_Pin pin, HD44780_PinMode mode);
  HD44780_Result (*write)(HD44780_GPIO_Interface *driver, HD44780_Pin pin, HD44780_PinState value);
  HD44780_Result (*read)(HD44780_GPIO_Interface *driver, HD44780_Pin pin, HD44780_PinState *value);
};

typedef void (*HD44780_AssertFn)(const char *filename, unsigned line);
typedef void (*HD44780_DelayMicrosecondsFn)(uint16_t us);

typedef enum
{
  HD44780_OPT_USE_RW = 1 << 0,
  HD44780_OPT_USE_BACKLIGHT = 1 << 1,
} HD44780_Options;

/* Hardware abstraction layer */
typedef struct
{
  HD44780_GPIO_Interface *gpios;
  HD44780_DelayMicrosecondsFn delay_microseconds;
  HD44780_AssertFn assert_failure_handler;
  HD44780_Options options;
} HD44780_Config;

/* HD44780 control structure */
typedef struct
{
  HD44780_Config cfg;

  uint8_t displayfunction;
  uint8_t displaycontrol;
  uint8_t displaymode;
  uint8_t initialized;
  uint8_t columns_amount;
  uint8_t lines_amount;
  uint8_t currline;

  HD44780_Pin dp_first;
  unsigned dp_amount;
} HD44780;

typedef enum { HD44780_MODE_4BIT, HD44780_MODE_8BIT } HD44780_Mode;

/***** User API *****/

HD44780_Result hd44780_init(HD44780 *display, HD44780_Mode mode,
    const HD44780_Config *config, uint8_t cols, uint8_t lines, uint8_t charsize);
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
HD44780_Result hd44780_backlight_on(HD44780 *display);
HD44780_Result hd44780_backlight_off(HD44780 *display);

/***** Low-level API *****/

HD44780_Result hd44780_config(HD44780 *display);
HD44780_Result hd44780_command(HD44780 *display, uint8_t value);
HD44780_Result hd44780_send(HD44780 *display, uint8_t value, HD44780_PinState rs_mode);
HD44780_Result hd44780_write_bits(HD44780 *display, uint8_t value);
HD44780_Result hd44780_read_bits(HD44780 *display, uint8_t *value);
HD44780_Result hd44780_pulse_enable_pin(HD44780 *display);

#define HD44780_MAKE_5BITS(b4,b3,b2,b1,b0) \
    (((b0) & 1) | \
     ((b1) & 1) << 1 | \
     ((b2) & 1) << 2 | \
     ((b3) & 1) << 3 | \
     ((b4) & 1) << 4)

#ifdef __cplusplus
}
#endif

#endif /* HC44780_H_ */
