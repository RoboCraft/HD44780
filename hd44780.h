#ifndef HC44780_H_
#define HC44780_H_

#define HD44780_USE_STM32F10X

#ifdef HD44780_USE_STM32F10X
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#endif

#include <stdint.h>
#include <stddef.h>

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

typedef struct
{
  GPIO_TypeDef *gpio;
  uint16_t gpio_pin;
} HD44780_Pin;

typedef struct
{
  HD44780_Pin rs;         // LOW: command.  HIGH: character.
  HD44780_Pin enable;     // activated by a HIGH pulse.
  HD44780_Pin rw;         // LOW: write to LCD.  HIGH: read from LCD.
  HD44780_Pin backlight;  // should be connected to base/gate of transistor/FET
  HD44780_Pin dp[8];      // data pins
} HD44780_Pinout;

typedef enum { HD44780_OK, HD44780_ERROR } HD44780_Result;
typedef enum { HD44780_PIN_INPUT, HD44780_PIN_OUTPUT } HD44780_PinMode;
typedef enum { HD44780_PIN_LOW, HD44780_PIN_HIGH } HD44780_PinState;

typedef struct
{
  HD44780_Result (*configure)(HD44780_Pin *pin, HD44780_PinMode mode);
  HD44780_Result (*write)(HD44780_Pin *pin, HD44780_PinState value);
} PinDriver;


#ifdef HD44780_USE_STM32F10X
HD44780_Result stm32f10x_default_pin_config(HD44780_Pin *pin, HD44780_PinMode mode);
HD44780_Result stm32f10x_default_pin_write(HD44780_Pin *pin, HD44780_PinState value);
extern const PinDriver STM32F10X_PinDriver;
#endif


/* Hardware abstraction layer */
typedef struct
{
  PinDriver pins;
  void (*delay_microseconds)(uint32_t us);
} HD44780_HAL;

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

HD44780_Result hd44780_init(HD44780 *display, HD44780_Mode mode,
    const HD44780_HAL *hal, const HD44780_Pinout *pinout,
    uint8_t cols, uint8_t lines, uint8_t charsize);
HD44780_Result hd44780_write(HD44780 *display, uint8_t value);
HD44780_Result hd44780_clear(HD44780 *display);
HD44780_Result hd44780_home(HD44780 *display);
HD44780_Result hd44780_display_off(HD44780 *display);
HD44780_Result hd44780_display_on(HD44780 *display);
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

HD44780_Result hd44780_config(HD44780 *display);
HD44780_Result hd44780_command(HD44780 *display, uint8_t value);
HD44780_Result hd44780_send(HD44780 *display, uint8_t value, uint8_t mode);
HD44780_Result hd44780_write8bits(HD44780 *display, uint8_t value);
HD44780_Result hd44780_write4bits(HD44780 *display, uint8_t value);
HD44780_Result hd44780_pulse_enable_pin(HD44780 *display);

#endif /* HC44780_H_ */
