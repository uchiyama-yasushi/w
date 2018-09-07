
/*-
 * Copyright (c) 2009 UCHIYAMA Yasushi.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Low-level interface of LCD.

#include <sys/system.h>
#include <sys/console.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/delay.h>

#include <string.h>
#include <reg.h>
#include <device/ad12864lcd.h>

#include "local.h"

struct pin_config {
  int port;
  int bit;
  int func;
  int mode;
  int dir;
};

enum lcd_pin
  {
    LCD_RESET,
    LCD_A0,
    LCD_LED_A,
    LCD_SCK,
    LCD_SDI,
    LCD_MISO,
    LCD_SSEL,
  };

#define	LCD_RESET_PORT	3
#define	LCD_RESET_BIT	26
#define	LCD_A0_PORT	1
#define	LCD_A0_BIT	30
#define	LCD_LED_PORT	1
#define	LCD_LED_BIT	31
#define	LCD_COMMAND()	GPIO_PIN_CLR (1, 30)
#define	LCD_DATA()	GPIO_PIN_SET (1, 30)


struct pin_config pin_config[] =
  {
    { LCD_RESET_PORT, LCD_RESET_BIT, 0, PIN_PULLUP, GPIO_OUTPUT },//GPIO #RESET
    { LCD_A0_PORT, LCD_A0_BIT, 0, PIN_PULLUP, GPIO_OUTPUT },//GPIO A0
    { LCD_LED_PORT, LCD_LED_BIT, 0, PIN_PULLUP, GPIO_OUTPUT },//GPIO LED_A
  };

void
lcd_display_init ()
{
  size_t i;
  struct pin_config *p = pin_config;

  spi_init ();

  for (i = 0; i < sizeof pin_config / sizeof (pin_config[0]); i++, p++)
    {
      mcu_pin_select (p->port, p->bit, p->func);
      mcu_pin_mode (p->port, p->bit, p->mode);
      if (p->dir != -1)
	gpio_dir (p->port, p->bit, p->dir);
    }
  udelay (200);
  GPIO_PIN_CLR (3, 26);
  udelay (200);
  GPIO_PIN_SET (3, 26);
  GPIO_PIN_SET (1, 31);
  udelay (200);

  lcd_command_putc (DISPLAY_ON);
  lcd_command_putc (POWER | POWER_BOOSTER | POWER_REGULATOR | POWER_FOLLOWER);
  lcd_command_putc (BRIGHTNESS);
  lcd_command_putc (0 & BRIGHTNESS_MASK);

  lcd_command_putc (REGULATOR | (7 & REGULATOR_MASK));
  lcd_command_putc (LCD_BIAS_1_9);
  lcd_command_putc (SCAN_DIR_REVERSE);
  lcd_command_putc (ADC_NORMAL);
  lcd_command_putc (ALL_POINT_OFF);
  lcd_command_putc (DISPLAY_REVERSE);
  lcd_command_putc (STATIC_INDICATOR_ON);
  lcd_command_putc (INDICATOR_OFF);
  lcd_command_putc (START_LINE | (0 & START_LINE_MASK));
  //  lcd_command_putc (READMODIFYWRITE_SET);
}

void
lcd_command_putc (int8_t c)
{
  LCD_COMMAND ();
  spi_putc (c);
}

void
lcd_data_putc (int8_t c)
{
  LCD_DATA ();
  spi_putc (c);
}

void
lcd_set_column (int i)
{
  LCD_COMMAND ();
  spi_putc (COLUMN_ADDR_HI | ((i >> 4) & 0xf));
  spi_putc (COLUMN_ADDR_LO | (i & 0xf));
}


void
lcd_set_page (int i)
{
  LCD_COMMAND ();
  spi_putc (PAGE_ADDR | (i & PAGE_ADDR_MASK));
}

void
lcd_contrast (int i)
{

  lcd_command_putc (BRIGHTNESS);
  lcd_command_putc (i & BRIGHTNESS_MASK);
}
