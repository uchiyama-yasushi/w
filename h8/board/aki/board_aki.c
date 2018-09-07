
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
#include <sys/board.h>

#include <reg.h>
#include <libaki/aki_led.h>
#include <libaki/aki_switch.h>
#include <libaki/aki_lcd.h>

STATIC void aki_device_init (void);
STATIC struct _file *lcdout;

void
aki_device_init ()
{

  lcdout = aki_lcd_init ();
  aki_led_init ();
  aki_switch_init ();
}

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE;
}

#ifndef OVERLAY
void
board_device_init (uint32_t arg __attribute__((unused)))
{

  aki_device_init ();
}
#endif

void
board_main (uint32_t arg __attribute__((unused)))
{
  char msg[] =  { 'A','K','I','-','H','8', ' ',
		   0xcf, 0xbb, 0xde, 0xb0, 0xce, 0xde, 0xb0, 0xc4, 0xde,'\0', };
  uint16_t r, r0;
  int i;
#ifdef OVERLAY
  aki_device_init ();
#endif
  aki_lcd_line (0);
  fprintf (lcdout, "%s", msg);

  r = r0 = 0;
  while (/*CONSTCOND*/1)
    {
      // Tactile switch
      r = (*P4_DR >> 4) & 0x0f;
      aki_led (0, r & (1 << 2));
      aki_led (1, r & (1 << 3));

      // DIP switch
      r |= (*P2_DR << 4) & 0xff0;

      if (r != r0)
	{
	  printf ("%x\n", r);

	  aki_lcd_line (1);
	  for (i = 0; i < 12; i++)
	    {
	      if (i == 4)
		fprintf (lcdout, " ");
	      fprintf (lcdout, "%d", r & (1 << i) ? 1 : 0);
	    }
	  r0 = r;
	}
    }
  /* NOTREACHED */
}
