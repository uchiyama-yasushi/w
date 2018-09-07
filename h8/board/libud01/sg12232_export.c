
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

/* AKIZUKI SG12232 122x32 graphic LCD (SUNLIKE) */
/* CAN EXPORT */

#include <sys/system.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include <libud01/sg12232.h>

STATIC struct character_screen_info
{
  int8_t page_start;
  int cx, cy;
  bool scroll;
  bool lastline;
} __s;

STATIC void lcd_clear_page (int8_t, uint8_t);
STATIC int lcd_cursor (int, int);
STATIC void lcd_font (int8_t, int8_t, int8_t);
STATIC void lcd_scroll (void);

void
lcd_clear (uint8_t pattern)
{
  int page;

  for (page = 0; page < LCD_PAGE_MAX; page++)
    {
      lcd_clear_page (page, pattern);
    }
  lcd_write (LCD_CMD_BOTH, CMD_START_LINE | pattern);

  // Also reset character screen.
  lcd_home ();
}

void
lcd_clear_page (int8_t page, uint8_t pattern)
{
  int column;

  lcd_write (LCD_CMD_BOTH, CMD_PAGE_ADDR | page);
  lcd_write (LCD_CMD_BOTH, CMD_COLUMN_ADDR | 0);
  for (column = 0; column < LCD_COLUMN_MAX; column++)
    lcd_write (LCD_DATA_BOTH, pattern);
}

int
lcd_cursor (int x, int y)
{
  int page, column, ic;

  if (x < 0 || x > 121 || y < 0 || y > 31)
    return -1;

  page = (y & ~7) >> 3;
  if (x > LCD_COLUMN_MAX - 1)	// LCD address [1:61]
    {
      column = x - LCD_COLUMN_MAX;
      ic = 1;
    }
  else
    {
      column = x;
      ic = 0;
    }

  LCD_CMD[ic] = CMD_PAGE_ADDR | page;
  LCD_CMD[ic] = CMD_COLUMN_ADDR | column;

  return ic;
}

//////////////////////////////////////////////////////////////////////
// Graphic interface.
//////////////////////////////////////////////////////////////////////
void
lcd_dot (int x, int y, bool on)
{
  int ic;

  if ((ic = lcd_cursor (x, y)) < 0)
    return;

  LCD_CMD[ic] = CMD_READ_MODIFY_WRITE; // don't increment column during read.
  (void)LCD_DATA[ic]; // dummy read is required when after page addr set.
  if (on)
    LCD_DATA[ic] = LCD_DATA[ic] | (1 << (y & 0x7));
  else
    LCD_DATA[ic] = LCD_DATA[ic] &  ~(1 << (y & 0x7));
  LCD_CMD[ic] = CMD_END;	// end read-modify-write mode.
}

// General integer Bresenham.
void
lcd_line (int x1, int y1, int x2, int y2)
{
  int16_t x, y, dx, dy, s1, s2, e, t, interchange, i;

  x = x1;
  y = y1;
  dx = abs (x2 - x1);
  dy = abs (y2 - y1);
  s1 = sign (x2 - x1);
  s2 = sign (y2 - y1);

  if (dy > dx)
    {
      t = dx;
      dx = dy;
      dy = t;
      interchange = TRUE;
    }
  else
    {
      interchange = FALSE;
    }
  e = 2 * dy - dx;

  for (i = 1; i <= dx; i++)
    {
      lcd_dot (x, y, TRUE);
      while (e >= 0)
	{
	  if (interchange)
	    x += s1;
	  else
	    y += s2;
	  e -= 2 * dx;
	}
      if (interchange)
	y += s2;
      else
	x += s1;
      e += 2 * dy;
    }
}

// General integer Bresenham.
void
lcd_circle (int r, int x0, int y0)
{
  int16_t xi, yi, di, d;

  xi = 0;
  yi = r;
  di = 2 * (1 - r);

  do
    {
      lcd_dot (x0 + xi, y0 + yi, TRUE);
      lcd_dot (x0 - xi, y0 + yi, TRUE);
      lcd_dot (x0 + xi, y0 - yi, TRUE);
      lcd_dot (x0 - xi, y0 - yi, TRUE);

      if (di < 0)
	{
	  d = 2 * di + 2 * yi - 1;
	  if (d <= 0)
	    {
	      xi++;
	      di = di + 2 * xi + 1;
	      continue;
	    }
	}
      else if (di > 0)
	{
	  d = 2 * di - 2 * xi - 1;
	  if (d > 0)
	    {
	      yi--;
	      di = di - 2 * yi + 1;
	      continue;
	    }
	}
      xi++;
      yi--;
      di = di + 2 * xi - 2 * yi + 2;
    }
  while (yi >= 0);
}

//////////////////////////////////////////////////////////////////////
// Character interface.
//////////////////////////////////////////////////////////////////////
void
lcd_font (int8_t c, int8_t cx, int8_t cy)
{
  int8_t i, ic, x, y;
  const uint8_t *font = font_clR8x8_data + c * 8;
  x = cx * 8;
  y = cy * 8;

  if ((ic = lcd_cursor (x, y)) < 0)
    return;

  if (cx == 7)	// across IC1, IC2
    {
      for (i = 0; i < 5; i++)
	LCD_DATA[ic] = font [i];
      ic = lcd_cursor (x + i, y);
      for (; i < 8; i++)
	LCD_DATA[ic] = font [i];
    }
  else
    {
      for (i = 0; i < 8; i++)
	LCD_DATA[ic] = font [i];
    }
}

void
lcd_scroll ()
{

  lcd_clear_page (__s.page_start++, 0x00);
  __s.page_start &= 3;
  lcd_write (LCD_CMD_BOTH, CMD_START_LINE | __s.page_start * 8);
}

void
lcd_putc (int8_t c)
{
  bool lf = (c == '\n');

  if (c == '\r')
    return;

  if (__s.scroll)
    {
      lcd_scroll ();
      __s.cx = 0;
      __s.scroll = FALSE;
    }

  if (!lf)
    lcd_font (c, __s.cx++, __s.cy);

  if (__s.cx > 14 || lf)
    {
      __s.cy++, __s.cx = 0;
      if (__s.lastline)
	__s.scroll = TRUE;
      if (__s.cy > 3)
	__s.scroll = TRUE, __s.lastline = TRUE;
      __s.cy &= 3;
    }
}

void
lcd_home ()
{

  memset (&__s, 0, sizeof __s);
}
