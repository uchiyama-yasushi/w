
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
#include <sys/timer.h>
#include <reg.h>

enum timer_channel
  {
    TIMER0,
    TIMER1,
    TIMER2,
    TIMER3,
    TIMER4,
    TIMER5,
  };

enum timer_cmd
  {
    TIMER_START,
    TIMER_STOP,
    TIMER_BUSY,
  };

STATIC addr_t timer_base[] = { T0_BASE, T1_BASE, T2_BASE,
			       T3_BASE, T4_BASE, T5_BASE };

bool tmu_start (int, enum timer_cmd);
intr_handler_t tmu0_intr;
intr_handler_t tmu1_intr;

static inline void
__reg_write_4 (enum timer_channel ch, addr_t offset, uint32_t val)
{
  *(volatile uint32_t *)(timer_base[ch] + offset) = val;
}

static inline uint32_t
__reg_read_4 (enum timer_channel ch, addr_t offset)
{
  return *(volatile uint32_t *)(timer_base[ch] + offset);
}

static inline void
__reg_write_2 (enum timer_channel ch, addr_t offset, uint32_t val)
{
  *(volatile uint16_t *)(timer_base[ch] + offset) = val;
}

static inline uint16_t
__reg_read_2 (enum timer_channel ch, addr_t offset)
{
  return *(volatile uint16_t *)(timer_base[ch] + offset);
}

void
timer_init ()
{
  // Install TMU0,1 interrupt handler.
  intr_table[0x580 >> 3] = tmu0_intr;
#ifndef THREAD_DISABLE
  intr_table[0x5a0 >> 3] = tmu1_intr;
#endif
  *INT2MSKR = 1;	// mask TMU0-2
  *INT2MSKCR = 1;	// unmask TMU0-2
  *INT2PRI0 |= 0x1f000000;// TMU0 highest priority.
#ifndef THREAD_DISABLE
  *INT2PRI0 |= 0x001f0000;// TMU1 highest priority
#endif

  iprintf ("%s: INTC: %x %x\n", *INT2MSKR, *INT2PRI0, __FUNCTION__);
}

bool
md_timer_start (int channel, timer_counter_t interval,
		enum timer_counter_unit counter_unit,
		enum timer_counter_match counter_conf)
{
  extern uint32_t udelay_param;
  uint16_t r;

  tmu_start (channel, TIMER_STOP);

  if (counter_unit == COUNTER_MSEC)
    interval *= 1000;
  interval *= udelay_param;
  __reg_write_4 (channel, T_CNT, interval);
  __reg_write_4 (channel, T_COR,
		 counter_conf == COUNTER_STOP ? 0 : interval);

  r = TPSC_PCK_4;
  if (channel < 2)
    r |= TCR_UNIE;	// Underflow interrupt.
  __reg_write_2 (channel, T_CR, r);

  return tmu_start (channel, TIMER_START);
}

void
md_timer_stop (int channel)
{

  tmu_start (channel, TIMER_STOP);
}

bool
md_timer_busy (int channel)
{

  return tmu_start (channel, TIMER_BUSY);
}

bool
tmu_start (int channel, enum timer_cmd cmd)
{
  volatile uint8_t *start_reg;
  uint8_t bit;

  if (channel < 3)
    {
      start_reg = TSTR0;
      bit = 1 << channel;
    }
  else
    {
      start_reg = TSTR1;
      bit = 1 << (channel - 3);
    }

  switch (cmd)
    {
    case TIMER_START:
      *start_reg |= bit;
      return TRUE;
    case TIMER_STOP:
      *start_reg &= ~bit;
      return TRUE;
    case TIMER_BUSY:
      return *start_reg & bit;
    }

  return FALSE;
}

void
tmu0_intr ()
{
  iprintf ("%s\n", __FUNCTION__);

  *TCR0 &= ~TCR_UNF;	// Clear underflow.
  tmu_start (TIMER0, TIMER_STOP);
  timer_do_schedule ();
}
#ifndef THREAD_DISABLE
void
tmu1_intr ()
{
  iprintf ("%s\n", __FUNCTION__);
  *TCR1 &= ~TCR_UNF;	// Clear underflow.
  tmu_start (TIMER1, TIMER_STOP);
  timer_do_wakeup ();
}
#endif
