
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
#include <reg.h>
#include <sys/delay.h>

uint32_t delay_param;

extern inline void
__timer_start ()
{
  *T0TC = 0;
  *T0TCR = TCR_ENABLE;
}


extern inline uint32_t
__timer_stop ()
{
  uint32_t r = *T0TC;

  *T0TCR &= ~TCR_ENABLE;
  return r;
}

void
delay_init ()
{
  cpu_status_t s = intr_suspend ();
  uint32_t t0, t1;

  __timer_config (TIMER0, 0, TIMER_USEC);

  delay_param = 1;
  // Measuere function call overhead.
  __timer_start ();
  udelay (0);
  t0 = __timer_stop ();

  __timer_start ();
  udelay (1000);
  t1 = __timer_stop ();
  delay_param = 1000 / (t1 - t0);

  intr_resume (s);
}

void
delay_calibrate ()
{
  uint32_t i;
  uint32_t t0;
  cpu_status_t s = intr_suspend ();

  //  iprintf ("delay_parm=%d\n", delay_param);
  __timer_config (TIMER0, 0, TIMER_USEC);

  for (i = 1000; i < 8000; i += 1000)
    {
      int lower = 0;
      while (1)
	{
	  __timer_start ();
	  udelay (i);
	  t0 = __timer_stop ();
	  //	  iprintf ("%d %d %d\n", t0, i, delay_param);
	  if (t0 <= i)
	    {
	      delay_param++;
	      lower = 1;
	    }
	  else
	    {
	      if (lower)
		break;
	      delay_param--;
	    }
	}
    }
  iprintf ("calibrated. delay_parm=%d\n", delay_param);
  intr_resume (s);
}

void
mdelay (uint32_t n)
{

  while (n--)
    udelay (1000); //1msec
}
