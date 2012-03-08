#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <misc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hd44780.h"
#include "hd44780_stm32f10x.h"
#include "utils.h"

void init_gpio(void);
void init_lcd(void);
void init_lcd2(void);

HD44780 lcd;
HD44780_STM32F10x_GPIO_Driver lcd_pindriver;

HD44780 lcd2;
HD44780_STM32F10x_GPIO_Driver lcd2_pindriver;

volatile uint32_t systick_ms = 0;

int main(void)
{
  SysTick_Config(SystemCoreClock / 1000);
  init_gpio();
  init_lcd();
  init_lcd2();

  while (1)
  {
    static uint32_t lcd_update_ms = 0;
    static int use_first_lcd = 1;

    if (uint32_time_diff(systick_ms, lcd_update_ms) >= 1000)
    {
      lcd_update_ms = systick_ms;

      static int counter = 0;

      const size_t buf_size = lcd.columns_amount + 1;
      char buf[buf_size];
      snprintf(buf, buf_size, "%d", counter);

      ++counter;

      if (use_first_lcd)
      {
        hd44780_clear(&lcd);
        hd44780_move_cursor(&lcd, 0, 0);
        hd44780_write_string(&lcd, buf);
      }
      else
      {
        hd44780_clear(&lcd2);
        hd44780_move_cursor(&lcd2, 0, 0);
        hd44780_write_string(&lcd2, buf);
      }

      use_first_lcd = !use_first_lcd;
    }
  }
}

void init_gpio(void)
{
}

void delay_microseconds(uint16_t us);
void hd44780_assert_failure_handler(const char *filename, unsigned line);

void init_lcd(void)
{
  const HD44780_STM32F10x_Pinout lcd_pinout =
  {
    {
      /* RS        */  { GPIOA, GPIO_Pin_5 },
      /* ENABLE    */  { GPIOA, GPIO_Pin_4 },
      /* RW        */  { GPIOC, GPIO_Pin_3 },
      /* Backlight */  { NULL, 0 },
      /* DP0       */  { NULL, 0 },
      /* DP1       */  { NULL, 0 },
      /* DP2       */  { NULL, 0 },
      /* DP3       */  { NULL, 0 },
      /* DP4       */  { GPIOA, GPIO_Pin_3 },
      /* DP5       */  { GPIOA, GPIO_Pin_2 },
      /* DP6       */  { GPIOA, GPIO_Pin_1 },
      /* DP7       */  { GPIOA, GPIO_Pin_0 },
    }
  };

  lcd_pindriver.interface = HD44780_STM32F10X_PINDRIVER_INTERFACE;
  lcd_pindriver.pinout = lcd_pinout;
  //lcd_pindriver.assert_failure_handler = hd44780_assert_failure_handler;

  const HD44780_Config lcd_config =
  {
    (HD44780_GPIO_Interface*)&lcd_pindriver,
    delay_microseconds,
    hd44780_assert_failure_handler,
  };

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  hd44780_init(&lcd, HD44780_MODE_4BIT, &lcd_config, 16, 2, HD44780_FLAG_5x8DOTS);
}

void init_lcd2(void)
{
  const HD44780_STM32F10x_Pinout lcd_pinout =
  {
    {
      /* RS        */  { GPIOA, GPIO_Pin_5 },
      /* ENABLE    */  { GPIOA, GPIO_Pin_6 },
      /* RW        */  { NULL, 0 },
      /* Backlight */  { NULL, 0 },
      /* DP0       */  { NULL, 0 },
      /* DP1       */  { NULL, 0 },
      /* DP2       */  { NULL, 0 },
      /* DP3       */  { NULL, 0 },
      /* DP4       */  { GPIOA, GPIO_Pin_3 },
      /* DP5       */  { GPIOA, GPIO_Pin_2 },
      /* DP6       */  { GPIOA, GPIO_Pin_1 },
      /* DP7       */  { GPIOA, GPIO_Pin_0 },
    }
  };

  lcd2_pindriver.interface = HD44780_STM32F10X_PINDRIVER_INTERFACE;
  lcd2_pindriver.pinout = lcd_pinout;
  //lcd2_pindriver.assert_failure_handler = hd44780_assert_failure_handler;

  const HD44780_Config lcd2_config =
  {
    (HD44780_GPIO_Interface*)&lcd2_pindriver,
    delay_microseconds,
    hd44780_assert_failure_handler
  };

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  hd44780_init(&lcd2, HD44780_MODE_4BIT, &lcd2_config, 8, 2, HD44780_FLAG_5x8DOTS);
}

void SysTick_Handler(void)
{
  ++systick_ms;
}

void delay_microseconds(uint16_t us)
{
  const uint32_t systick_ms_start = systick_ms;

  while (1)
  {
    uint32_t diff = (systick_ms >= systick_ms_start ?
        systick_ms - systick_ms_start : UINT32_MAX - systick_ms_start + systick_ms);

    if (diff >= (uint32_t)us / 1000)
      break;
  }
}

void hd44780_assert_failure_handler(const char *filename, unsigned line)
{
  do __NOP(); while (1);
}
