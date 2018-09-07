
/*-
 * Copyright (c) 2008-2009 UCHIYAMA Yasushi.  All rights reserved.
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

// H8/3052

#include <sys/system.h>

#include <reg.h>
#include <sys/delay.h>
#include <sys/console.h>

extern uint16_t delay_mult, delay_div;	// delay_subr.S
STATIC void delay_setparam (uint16_t, uint16_t);
#define	EXTERN_INLINE static inline

EXTERN_INLINE void
__timer_start ()
{
  *ITU0_TCNT = 0;
  ITU_START (0);
}

EXTERN_INLINE uint16_t
__timer_stop ()
{
  uint16_t t = *ITU0_TCNT;
  ITU_STOP (0);
  return t;
  return 0;
}

void
mdelay (uint32_t n)
{

  while (n--)
    udelay (1000); //1msec
}

void
delay_setparam (uint16_t div, uint16_t mult)
{

  delay_mult = mult;
  delay_div = div;
}

void
delay_init ()
{
  uint16_t t0, t1;
  cpu_status_t s = intr_suspend ();

  // determine delay_div.
  *ITU_TSTR = 0;	// All timer stop.
  // Internal clock/2, Don't clear TCNT.
  *ITU0_TCR = ITU__TCR_ICLK2;	// .08us (CPU 25MHz) ->OVF 5.242 msec
  *ITU0_TIOR = 0;	// no I/O
  *ITU0_TIER = 0;	// no interrupt.

  delay_setparam (1, 1);
  // Measuere function call overhead.
  __timer_start ();
  udelay (0);
  t0 = __timer_stop ();
  // Determine delay parameter.
  __timer_start ();
  udelay (1000);
  t1 = __timer_stop ();
  delay_setparam ((t1 - t0) / 100, 125);

  intr_resume (s);
}

void
delay_calibrate ()
{
  uint32_t i, j;
  uint16_t t0;
  cpu_status_t s = intr_suspend ();

  iprintf ("delay_div=%d\n", delay_div);

  *ITU0_TCR = ITU__TCR_ICLK8;	// .32us (CPU 25MHz) ->OVF 20.970msec
  for (i = 1000; i < 8000; i += 1000)
    {
      int lower = 0;
      // t0 should be i usec/0.32usec. = i * 100/32
      j = i * 100 / 32;
      while (1)
	{
	  __timer_start ();
	  udelay (i);
	  t0 = __timer_stop ();
	  //printf ("%d %d %d\n", t0, j, delay_div);
	  if (t0 <= j)
	    {
	      delay_div--;
	      lower = 1;
	    }
	  else
	    {
	      if (lower)
		break;
	      delay_div++;
	    }
	}
    }
  iprintf ("calibrated. delay_div=%d\n", delay_div);
  intr_resume (s);
}
