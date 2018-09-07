
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

// LCD application code.

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>
#include <string.h>
#include <device/ad12864lcd.h>
#include "local.h"
#include "data.h"
#include "lcd.h"

//#define	TEST_DISP

static void prepare_laptime_buffer (char *, uint32_t/*msec*/);

void
lcd_app_init ()
{
  int i;
  // Enable LCD
  lcd_display_init ();
  lcd_contrast (0);

  // Clear VRAM
  for (i = 0; i < VRAM_NPAGE; i++)
    lcd_vram_clear_page (i);
  // Transfer to LCD.
  lcd_vram_transfer ();
  // Test display
  lcd_data_laptime (599999);
  lcd_data_elapsed (15);
}

void
lcd_data_laptime (uint32_t total_lap) //msec
{
  char buf[8];
  int i;

  memset (buf, 0, sizeof buf);
  prepare_laptime_buffer(buf, total_lap);

  lcd_vram_clear_page (4);
  lcd_vram_clear_page (5);
  lcd_vram_clear_page (6);

  //  iprintf("%s:%s\n", __FUNCTION__, buf);
  for (i = 0; i < 5; i++)
    {
      int x = i * 24;
      char c;
      if (i > 0)
	x += 5;
      if (i > 2)
	x += 5;
      if ((c = buf[i]))
	{
	  lcd_vram_font (c - '0', x, 4, FALSE);
	  lcd_vram_font (c - '0', x + 1, 4, FALSE);//BOLD
	}
      if (i == 0)
	lcd_vram_font (11, x + 18, 4, FALSE);
      if (i == 2)
	lcd_vram_font (10, x + 18, 4, FALSE);
    }
  lcd_vram_transfer ();
}

void
prepare_laptime_buffer (char *p, uint32_t lap/*msec*/)
{
  int min, sec, sec_h, sec_l, msec_h, msec_l;

  min = lap / (1000 * 60);
  lap -= min * 1000 * 60;
  min %= 10;// 1 digit

  sec = lap / 1000;
  lap -= sec * 1000;
  sec_h = sec / 10;
  sec_l = sec - sec_h * 10;
  msec_h = lap / 100;
  lap -= msec_h * 100;
  msec_l = lap / 10;

  if (min)
    *p = '0' + min;
  p++;
  if (sec_h)
    *p = '0' + sec_h;
  p++;
  if (sec_h || sec_l)
    *p = '0' + sec_l;
  p++;
  *p++ = '0' + msec_h;
  *p = '0' + msec_l;
}

void
lcd_data_misc (int data0, int data1)
{
  char *p, buf[16];
  const char digits[] = "0123456789";
  int i;

  memset (buf, 0, sizeof buf);
  p = buf;
  for (i = 0; i < 2; i++, p++, data1 /= 10)
    {
      char q = digits[data1 % 10];
      if (q && data1)
	*p = q;
    }


  for (i = 0; i < 3; i++, p++, data0 /= 10)
    {
      char q = digits[data0 % 10];
      if (q && data0)
	*p = q;
    }

  //  iprintf ("%d %d\n", elapsed, current_speed);
  lcd_vram_clear_page (0);
  lcd_vram_clear_page (1);
  lcd_vram_clear_page (2);

  for (i = 0; i < 5; i++)
    {
      int x = i * 24;
      char c = *--p;
      if (c) {
	//	iprintf ("[%c]\n", c);
	lcd_vram_font (c - '0', x, 0, i < 3);
      }
    }

  lcd_vram_transfer ();
}

void
lcd_data_elapsed (int min)
{
  uint8_t *p = lcd_vram_get_page (7);
  lcd_vram_clear_page (7);

  memset (p, 0xf0, (128 * min / 30) & 0x7f);
  lcd_vram_transfer ();
}

void
lcd_data_gps_status ()
{
  static int status;
  uint8_t *p = lcd_vram_get_page (3);
  lcd_vram_clear_page (3);

  status = (status + 1) & 0x7f;
  memset (p, 0x18, status);
  lcd_vram_transfer ();
}
