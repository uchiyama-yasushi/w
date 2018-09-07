
/*-
 * Copyright (c) 2009, 2011 UCHIYAMA Yasushi.  All rights reserved.
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
#include <pcat/bios.h>
#include <string.h>

// for interrupt context or no threading support.
CONSOLE_OUT_DECL (vga);

void vga_console_init (void);
void vga_putc_subr (int, int, int);
void vga_scroll (void);

//'#define	BPP8
#ifdef BPP8
#define	XRES	320
#define	YRES	200
#define	XMAX	39
#define	YMAX	24
#define	LINEDOT	320
#else
#define	XRES	640
#define	YRES	480
#define	XMAX	79
#define	YMAX	59
#define	LINEDOT	80
#endif

void
vga_console_init ()
{

  // VGA Graphic.
#ifdef BPP8
  //	0x13: 320x240 8bit color VRAM 0xa0000 - 0xafa00
  call16 (bios_vga_mode, 0x13, 0);
#else
  //	0x12: 640x480 4bit color
  call16 (bios_vga_mode, 0x12, 0);
#endif
  stdout->char_ops = &vga_console_ops;
}

void
vga_putc (void *arg __attribute__((unused)), int8_t c)
{
  static int __x , __y;
  int lf = (c == '\n');

  if (c == '\r')
    return;
  if (!lf)
    vga_putc_subr (c, __x++, __y);

  if (__x > XMAX || lf)
    __y++, __x = 0;

  if (__y > YMAX)
    {
      __y = YMAX;
      vga_scroll ();
    }
}

void
vga_scroll ()
{
  uint8_t (*vram)[LINEDOT] = (void *)0xa0000;

  memmove (vram[0], vram[8], (YRES - 8) * LINEDOT);
  memset (vram[YRES - 8], 0, 8 * LINEDOT);
}

void
vga_putc_subr (int c, int x0, int y0)
{
  extern unsigned char font_clR8x8_data[];
  uint8_t (*vram)[LINEDOT] = (void *)0xa0000;
  uint8_t *f = font_clR8x8_data + c * 8;
#ifdef BPP8
  int x, y;
  x0 *= 8;
  y0 *= 8;

  for (y = 0; y < 8; y++)
    {
      uint8_t bitmap = f[y];
      for (x = 0; x < 8; x++)
	{
	  // 0 black
	  // 1 blue
	  // 2 green
	  // 3 cyan
	  // 4 red
	  // 5 magenta
	  // 6 yellow
	  // 7 white
	  // 8 gray
	  vram[y0 + y][x0 + x] = (bitmap & (1 << (7-x))) ? 7 : 0;
	}
    }
#else
  int  y;

  y0 *= 8;
  for (y = 0; y < 8; y++)
    {
      vram[y0 + y][x0] = f[y];
    }
#endif
}
