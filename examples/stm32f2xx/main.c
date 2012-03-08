#include <stm32f2xx.h>
#include <stm32f2xx_rcc.h>
#include <stm32f2xx_gpio.h>

#include <stdio.h>
#include <stdlib.h>

#include "hd44780.h"
#include "hd44780_stm32f2xx.h"

void init_lcd(void);
void delay_microseconds(uint16_t us);
uint32_t uint32_time_diff(uint32_t now, uint32_t before);
void hd44780_assert_failure_handler(const char *filename, unsigned long line);

HD44780 lcd;
HD44780_STM32F2xx_GPIO_Driver lcd_pindriver;
volatile uint32_t systick_ms = 0;

int main()
{
  SysTick_Config(SystemCoreClock / 1000);
  init_lcd();

  while (1)
  {
    static uint32_t lcd_update_ms = 0;

    if (uint32_time_diff(systick_ms, lcd_update_ms) >= 1000)
    {
      lcd_update_ms = systick_ms;

      static unsigned counter = 0;

      const size_t buf_size = lcd.columns_amount + 1;
      char buf[buf_size];
      snprintf(buf, buf_size, "%d", counter);

      ++counter;

      hd44780_clear(&lcd);
      hd44780_move_cursor(&lcd, 0, 0);
      hd44780_write_string(&lcd, buf);
    }
  }
}

void SysTick_Handler(void)
{
  ++systick_ms;
}

void init_lcd(void)
{
  const HD44780_STM32F2xx_Pinout lcd_pinout =
  {
    {
      /* RS        */  { GPIOD, GPIO_Pin_15 },
      /* ENABLE    */  { GPIOD, GPIO_Pin_13 },
      /* RW        */  { GPIOD, GPIO_Pin_11 },
      /* Backlight */  { NULL, 0 },
      /* DP0       */  { NULL, 0 },
      /* DP1       */  { NULL, 0 },
      /* DP2       */  { NULL, 0 },
      /* DP3       */  { NULL, 0 },
      /* DP4       */  { GPIOD, GPIO_Pin_9 },
      /* DP5       */  { GPIOB, GPIO_Pin_15 },
      /* DP6       */  { GPIOB, GPIO_Pin_13 },
      /* DP7       */  { GPIOB, GPIO_Pin_11 },
    }
  };

  lcd_pindriver.interface = HD44780_STM32F2XX_PINDRIVER_INTERFACE;
  lcd_pindriver.pinout = lcd_pinout;
  lcd_pindriver.assert_failure_handler = hd44780_assert_failure_handler;

  const HD44780_Config lcd_config =
  {
    (HD44780_GPIO_Interface*)&lcd_pindriver,
    delay_microseconds,
    hd44780_assert_failure_handler,
    HD44780_OPT_USE_RW
  };

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  hd44780_init(&lcd, HD44780_MODE_4BIT, &lcd_config, 16, 2, HD44780_CHARSIZE_5x8);
}

void delay_microseconds(uint16_t us)
{
  SysTick->VAL = SysTick->LOAD;
  const uint32_t systick_ms_start = systick_ms;

  while (1)
  {
    uint32_t diff = uint32_time_diff(systick_ms, systick_ms_start);

    if (diff >= ((uint32_t)us / 1000) + (us % 1000 ? 1 : 0))
      break;
  }
}

uint32_t uint32_time_diff(uint32_t now, uint32_t before)
{
  return (now >= before) ? (now - before) : (UINT32_MAX - before + now);
}

void hd44780_assert_failure_handler(const char *filename, unsigned long line)
{
  (void)filename; (void)line;
  do {} while (1);
}
