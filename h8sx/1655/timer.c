
/*-
 * Copyright (c) 2010 UCHIYAMA Yasushi.  All rights reserved.
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

// MI timer interface. use TMR (8bit)

#include <sys/system.h>
#include <sys/console.h>
#include <sys/timer.h>

#include <reg.h>
#include <frame.h>

#include <string.h>

#ifdef TIMER_DEBUG
#define	DPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

// Convert MI channel to TMR channel.
#define	TMR_CHANNEL(x)	(7 - (x))

void
timer_init ()
{
  // Interrupt setting.
  tmr_init (TMR_CH7, INTPRI_7); // Highest.
  tmr_init (TMR_CH6, INTPRI_7); // Highest.
}

bool
md_timer_start (int channel, timer_counter_t interval,
		enum timer_counter_unit counter_unit,
		enum timer_counter_match counter_conf)
{
  struct tmr_config config;
  int usec;
  uint8_t cnt;

  if (md_timer_busy (channel))
    return FALSE;

  // Set counter match.
  usec = interval;
  if (counter_unit == COUNTER_MSEC)
    {
      usec = interval  * 1000;
    }
  memset (&config, 0, sizeof config);
  if ((cnt = tmr_constant_calc (&config, usec)) == 0)
    {
      iprintf ("XXX Timer counter overflow.\n");
      return FALSE;
    }

  DPRINTF ("%d usec: TCR=%x TCCR=%x cnt=%d\n", usec, config.TCR, config.TCCR,
	   cnt);

  config.TCORA = cnt;

  // Counter reset.
  if (counter_conf == COUNTER_STOP)
    config.TCR |= TCR_CCLR_NO;
  else
    config.TCR |= TCR_CCLR_CMP_A;

  // Interrupt enable.
  config.TCR |= TCR_CMIEA;

  // Start timer.
  tmr_start (TMR_CHANNEL (channel), &config);

  return TRUE;
}

void
md_timer_stop (int channel)
{

  tmr_stop (TMR_CHANNEL (channel));
}

bool
md_timer_busy (int channel)
{

  return tmr_busy (TMR_CHANNEL (channel));
}

void
#if defined THREAD_DISABLE || !defined EXT_FRAME_INFO
c_tmr7 ()
#else
c_tmr7 (struct exception_frame *ef,
	uint8_t opri __attribute__((unused)),
	uint8_t pri __attribute__((unused)))
#endif
{
  *TMR (7, TCR) = TMR_CKS_NOCLK; // Stop timer
  *TMR (7, TCSR) &= ~TCSR_CMFA;
  *TMR (7, TCSR); // dummy read.
  *TMR (7, TCNT) = 0;
#ifndef THREAD_DISABLE
  ef->thread_state = THREAD_STATE_INTR_TMR7;
#endif
#if defined TIMER_DEBUG && defined EXT_FRAME_INFO
  frame_exception_assert (ef, opri, pri);
  iprintf ("%s\n", __FUNCTION__);
#endif

  timer_do_schedule ();
}

void
#if defined THREAD_DISABLE || !defined EXT_FRAME_INFO
c_tmr6 ()
#else
c_tmr6 (struct exception_frame *ef,
	uint8_t opri __attribute__((unused)),
	uint8_t pri __attribute__((unused)))
#endif
{
  *TMR (6, TCR) = TMR_CKS_NOCLK; // Stop timer
  *TMR (6, TCSR) &= ~TCSR_CMFA;
  *TMR (6, TCSR); // dummy read.
  *TMR (6, TCNT) = 0;
#ifndef THREAD_DISABLE
  ef->thread_state = THREAD_STATE_INTR_TMR6;
#endif
#if defined TIMER_DEBUG && defined EXT_FRAME_INFO
  frame_exception_assert (ef, opri, pri);
  iprintf ("%s\n", __FUNCTION__);
#endif
#ifndef THREAD_DISABLE
  timer_do_wakeup ();
#endif
}

