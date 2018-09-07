
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
#include <reg.h>
#include <sys/timer.h>

bool
md_timer_start (int channel, timer_counter_t interval/*usec*/,
		enum timer_counter_unit unit __attribute__((unused)),
		enum timer_counter_match match __attribute__((unused)))
{
  uint32_t count;

  if (md_timer_busy (channel))
    return FALSE;	// busy;

  count = interval;
  __asm volatile ("mulxu %2, %0 \n divxu.w %3, %0"
		  : "=r" (count) : "0" (count), "r" (25), "r" (8));
  // count = interval * 25 / 8;

  if (channel)
    {
      *ITU1_TCR = ITU__TCR_ICLK8;	// 3.125MHz
      *ITU1_TIER |= ITU__TIER_IMIEA;
      *ITU1_GRA = (uint16_t)count;
      *ITU1_TCNT = 0;
      ITU_START (1);
    }
  else
    {
      *ITU0_TCR = ITU__TCR_ICLK8;	// 3.125MHz	0.32usec/clock
      *ITU0_TIER |= ITU__TIER_IMIEA;
      *ITU0_GRA = count;
      *ITU0_TCNT = 0;
      ITU_START (0);
    }
  return TRUE;
}

bool
md_timer_busy (int channel)
{

  return  channel ? *ITU1_TIER & ITU__TIER_IMIEA : *ITU0_TIER & ITU__TIER_IMIEA;
}

void
md_timer_stop (int channel)
{

  if (channel)
    ITU_STOP (1);
  else
    ITU_STOP (0);
}
