
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

#include <sys/system.h>
#include <sys/console.h>
#include <sys/delay.h>
#include <pcat/bios.h>

// This routine may be called before bss clear.
static int mdelay_cnt __attribute__((section (".data")));
static int udelay_cnt __attribute__((section (".data")));

static   uint16_t cnt;

void
delay_init ()
{
  int delay_cnt;
  int i;

  for (i = 0; i < 16; i++)
    {
      if ((cnt = call16 (bios_delay_calibration, 1 << i, 0)) != 0)
	break;
    }

  // 55msec.(65536/1193180sec)
  delay_cnt = 1 << i;
#if 0
  int adjust, lower = 0;
  adjust = delay_cnt / 10;
  while (/*CONSTCOND*/1)
    {
      if (call16 (bios_delay_calibration, adjust, 0) == 0)
	{
	  delay_cnt +=  adjust;
	  lower = 1;
	}
      else
	{
	  if (lower)
	    break;
	  delay_cnt -= adjust;
	}
    }
#endif
  mdelay_cnt = delay_cnt * 1193180 / 1000;
  udelay_cnt = mdelay_cnt / 1000;
}

void
delay_calibrate ()
{
  int i;

  printf ("%d m:%d u%d\n", cnt, mdelay_cnt, udelay_cnt);

  for (i = 0; i < 3; i++)
    {
      delay_init ();
      printf ("[%d]\n", i);
      mdelay (1000);
    }
}

void
udelay (uint16_t cnt)
{

  bios_delay32 (udelay_cnt * cnt);
}

void
mdelay (uint32_t cnt)
{
  //  int i;

  bios_delay32 (mdelay_cnt * cnt);
}
