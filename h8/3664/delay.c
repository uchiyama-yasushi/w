
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

//H8/3664 16MHz , H8/3694 20MHz

#include <sys/system.h>

#include <reg.h>
#include <sys/delay.h>
#include <sys/console.h>

extern uint16_t delay_mult, delay_div;	// delay_subr.S
void delay_setparam (uint16_t, uint16_t);
#define	EXTERN_INLINE	static inline

EXTERN_INLINE void
__timer_v_start ()
{
  // 16MHz/128 -> Overflow = 128/16*256 = 2048usec
  // 20MHz/128 -> Overflow = 128/20*256 = 1638.4usec
  *TCRV1 = TCRV1_ICKS0;		// internal clock/128
  *TCRV0 = TCRV0_CKS_DIV128INT;	// internal clock/128
  *TCNTV = 0;
}

// Timer V ops
EXTERN_INLINE uint8_t
__timer_v_count ()
{

  return *TCNTV;
}

EXTERN_INLINE void
__timer_v_stop ()
{

  *TCRV0 = TCRV0_CKS_NONE;	// Counter stop.
}

// Timer W ops
EXTERN_INLINE void
__timer_w_start ()
{
  // Initialize.
  *TMRW = 0;	// Compare match. Counter stop
  *TCRW	= TCRW_CKS_DIV8; // Dont' clear counter. system clock/8
  *TSRW;	// dummy read;
  *TSRW = 0;	// clear interrupt status.
  *TIERW = 0;	// no interrupt.

  // Start.
  *TCNT = 0;
  *TMRW |= TMRW_CTS; // Conter start.
}

EXTERN_INLINE uint16_t
__timer_w_count ()
{

  return *TCNT;
}

EXTERN_INLINE void
__timer_w_stop ()
{

  *TMRW &= ~TMRW_CTS; // Conter stop
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

// Estimate delay parmeter with Timer V
void
delay_init ()
{
  uint8_t t0, t1;
  cpu_status_t s = intr_suspend ();

  delay_setparam (1, 1);
  // Measuere function call overhead.
  __timer_v_start ();
  udelay (0);
  t0 = __timer_v_count ();
  __timer_v_stop ();

  // Determine delay parameter.
  __timer_v_start ();
  udelay (1000);
  t1 = __timer_v_count ();
  __timer_v_stop ();

#if SYSTEM_CLOCK == 16
  delay_setparam (t1 - t0, 125);
#elif SYSTEM_CLOCK == 20
  delay_setparam ((t1 - t0) * 4, 625);
#else
#error
#endif

  intr_resume (s);
  /*
    - 16MHz - divider x
    16MHz/x -> 1count = x/16 usec.
    1000 loop = (t1 - t0) * x/16 usec.
    1usec = (1000 * 16)/(x * (t1 - t0))

    x = 32 t = t1-t0
    1usec = 500/t
    x = 128
    1usec = 125/t (t->divider, 125->multiplier)

    - 20MHz - divider x
    1usec = (1000 * 20)/(x * (t1 - t0))

    x = 32 t = t1-t0
    1usec = 625/t
    x = 128
    1usec = 156.25/t (t->divider, 125->multiplier)
          = 625 / (t * 4)
   */
}

// Calibrate delay parameter with Timer W
void
delay_calibrate ()
{
#ifndef DELAY_CALIBRATE_DISABLE
  uint32_t i, j;
  uint32_t t0;
  cpu_status_t s = intr_suspend ();

  iprintf ("delay_div=%d\n", delay_div);

  for (i = 1000; i < 8000; i += 1000)
    {
      int lower = 0;
#if SYSTEM_CLOCK == 16
      // 16MHz -> .5usec/count.
      // t0 should be i usec/0.5usec. = i * 2
      j = i * 2;
#elif SYSTEM_CLOCK == 20
      // 20MHz -> .4usec/count.
      // t0 should be i usec/0.4usec. = i * 5 / 2
      j = i * 5 / 2;
#else
#error
#endif
      while (1)
	{
	  __timer_w_start ();
	  udelay (i);
	  t0 = __timer_w_count ();
	  __timer_w_stop ();
	  //	  iprintf ("%d %d %d\n", t0, j, delay_div);
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
#endif //!DELAY_CALIBRATE_DISABLE
}
