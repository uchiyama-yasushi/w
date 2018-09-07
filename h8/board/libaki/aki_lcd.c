
/*-
 * Copyright (c) 2008 UCHIYAMA Yasushi.  All rights reserved.
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

#include <sys/system.h>
#include <sys/types.h>
#include <sys/console.h>
#include <reg.h>
#include <sys/delay.h>
#include <libaki/aki_lcd.h>

// SC1602 16 character x 2 line LCD module.  LCD controler: HD44780

// Register select. 0: instruction 1: data
#define	RS_BIT		(1 << 4)
#define	E_BIT		(1 << 5)
#define	RS_SET()	(*P3_DR |= RS_BIT)
#define	RS_CLR()	(*P3_DR &= ~RS_BIT)
#define	E_SET()		(*P3_DR |= E_BIT)
#define	E_CLR()		(*P3_DR &= ~E_BIT)
#define	DELAY_40	40

// Write
#define	LCD_CLEAR		0x01	// 1.64ms
#define	LCD_CURSOR_HOME		0x02	// 1.64ms
#define	LCD_ENTRY_MODE_SET	0x04	// 40us
#define	 LCD_ENTRY_CURSOR_INC	 0x02	// 0: decriment
#define	 LCD_ENTRY_DISPLAY_SHIFT 0x01	// 0: don't shift.
#define	LCD_ON			0x08	// 40us
#define	 LCD_ON_DISPLAY		 0x04
#define	 LCD_ON_CURSOR		 0x02
#define	 LCD_ON_BLINK		 0x01
#define	LCD_SHIFT		0x10	// 40us
#define	 LCD_SHIFT_DISPLAY	 0x08	// 0: shift cursor
#define	 LCD_SHIFT_RIGHT	 0x04	// 0: left
#define	LCD_FUNCTION_SET	0x20	// 40us
#define	 LCD_FUNCTION_8BIT	 0x10	// 0: 4bit
#define	 LCD_FUNCTION_2LINE	 0x08	// 0: 1line
#define	 LCD_FUNCTION_FONT_5x10	 0x04	// 0: 5x7
#define	LCD_CGADDR		0x40	// 40us Character Generator RAM
#define	LCD_DDADDR		0x80	// 40us Display Data RAM
// Read
#define	LCD_BUSY		0x80
#define	LCD_ADDR_CNT_MASK	0x7f

static void aki_lcd_cmd (uint8_t);
static void aki_lcd_data (uint8_t);
static void aki_lcd_out4 (uint8_t);

CONSOLE_OUT_DECL (aki_lcd);

STATIC struct _file lcdout;

struct _file *
aki_lcd_init ()
{
  // Set port 3 output.
  *P3_DDR = 0xff;

  // Wait 15ms for LCD stabilize.
  mdelay (15);
  // 4bit-interface software reset.
  *P3_DR = E_BIT | 0x3; udelay (40); E_CLR ();
  mdelay (4);
  *P3_DR = E_BIT | 0x3; udelay (40); E_CLR ();
  udelay (100);
  *P3_DR = E_BIT | 0x3; udelay (40); E_CLR ();
  *P3_DR = E_BIT | 0x2; udelay (40); E_CLR ();
  // Instruction acceptable now.

  // 4bit data-length, 2line, 5x7 font.
  aki_lcd_cmd (LCD_FUNCTION_SET | LCD_FUNCTION_2LINE);
  // don't shift display.
  aki_lcd_cmd (LCD_ENTRY_MODE_SET | LCD_ENTRY_CURSOR_INC);
  // Set CGRAM Addr
  aki_lcd_cmd (LCD_CGADDR | 0x0);
  // Set DDRAM Addr
  aki_lcd_cmd (LCD_DDADDR | 0x0);
  // Display on
  aki_lcd_cmd (LCD_ON | LCD_ON_DISPLAY); // LCD_ON_CURSOR | LCD_ON_BLINK

  // LCD clear
  aki_lcd_cmd (LCD_CLEAR);
  mdelay (4);

  lcdout.char_ops = &aki_lcd_console_ops;

  return &lcdout;
}

void
aki_lcd_cmd (uint8_t cmd)
{
  RS_CLR ();
  aki_lcd_out4 (cmd);
}

void
aki_lcd_data (uint8_t data)
{
  RS_SET ();
  aki_lcd_out4 (data);
  RS_CLR ();
}

void
aki_lcd_out4 (uint8_t cmd)
{

  E_SET ();
  *P3_DR = (*P3_DR & 0xf0) | ((cmd >> 4) & 0xf);
  udelay (DELAY_40);
  E_CLR ();
  udelay (DELAY_40);

  E_SET ();
  *P3_DR = (*P3_DR & 0xf0) | (cmd & 0xf);
  udelay (DELAY_40);
  E_CLR ();
  udelay (DELAY_40);
}

void
aki_lcd_line (int line)
{

  aki_lcd_cmd (line ? (LCD_DDADDR | 0x40) : (LCD_DDADDR | 0x0));
}

void
aki_lcd_putc (void *ctx __attribute__((unused)), int8_t c)
{

  aki_lcd_data (c);
}
