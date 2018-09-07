
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

// H8/3664 16MHz, H8/3694 20MHz.

#include <sys/system.h>
#include <reg.h>
#include <sys/timer.h>
#include <sys/console.h>

STATIC bool timer_w_start (timer_counter_t);
STATIC bool timer_w_busy (void);
STATIC bool timer_v_start (timer_counter_t);
STATIC bool timer_v_busy (void);

bool
md_timer_start (int channel, timer_counter_t interval,
		enum timer_counter_unit unit __attribute__((unused)),
		enum timer_counter_match match __attribute__((unused)))
{

  return channel ? timer_w_start (interval) : timer_v_start (interval);
}

void
md_timer_stop (int channel)
{
  channel = channel;
  assert (0); // notyet.
}

bool
md_timer_busy (int channel)
{

  return channel ? timer_w_busy () : timer_v_busy ();
}

bool
timer_w_start (timer_counter_t interval/*usec*/)
{
  uint8_t r;
  uint32_t count;

  if (timer_w_busy ())
    return FALSE;
  //clock divider   MHz     usec/clock      overflow(msec)
  //16	 8	2.00	0.500000	32.768002
  //20	 8	2.50	0.400000	26.214401
#if SYSTEM_CLOCK == 16
  count = interval * 2;
#elif SYSTEM_CLOCK == 20
  count = interval * 5 / 2;
#else
#error
#endif
  //  iprintf ("%s: %d\n", __FUNCTION__, count);
  assert (count <= 0xffff);

  *TMRW = 0;	// Compare match. Counter stop
  *TCRW	= TCRW_CKS_DIV8; // Dont' clear counter. system clock/8
  r = *TSRW;	// dummy read;
  r = r;
  *TSRW = 0;	// Clear status.

  *TIERW = TIERW_IMIEA;
  *GRA = (uint16_t)count;
  *TCNT = 0;
  *TMRW |= TMRW_CTS; // Conter start.

  return TRUE;
}

bool
timer_w_busy ()
{

  return *TIERW & TIERW_IMIEA;
}

bool
timer_v_start (uint16_t interval/*usec*/)
{
  uint32_t count;

  //    clock divider   MHz     usec/clock      overflow(msec)
  //    16	128	0.12	8.000000	2.048000
  //	20	128	0.16	6.400000	1.638400
#if SYSTEM_CLOCK == 16
  count = interval * 3 / 25;
#elif SYSTEM_CLOCK == 20
  count = interval * 4 / 25;
#else
#error
#endif
  //  iprintf ("%s: %d\n", __FUNCTION__, count);
  assert (count <= 0xff);

  *TCORA = (uint8_t)count;
  *TCNTV = 0;
  *TCRV1 = TCRV1_ICKS0;		// internal clock/128
  *TCRV0 = TCRV0_CKS_DIV128INT | TCRV0_CMIEA;
  return TRUE;
}

bool
timer_v_busy ()
{

  return *TCRV0 & TCRV0_CMIEA;
}
