
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

__BEGIN_DECLS
// Interrupt handler. registered to irq_vector[]@interrupt.c
void timer0_intr (void);
void timer1_intr (void);
void timer2_intr (void);
__END_DECLS

STATIC addr_t timer_base[] = { T0_BASE, T1_BASE, T2_BASE, T3_BASE };
STATIC enum pclk_select clock_bits[] =
  { PCLK_TIMER0, PCLK_TIMER1, PCLK_TIMER2, PCLK_TIMER3 };
STATIC uint32_t power_bits[] =
  { PCONP_PCTIM0, PCONP_PCTIM1, PCONP_PCTIM2, PCONP_PCTIM3 };
STATIC uint32_t vic_bits[] =
  { VIC_TIMER0, VIC_TIMER1, VIC_TIMER2, VIC_TIMER3 };

__BEGIN_DECLS
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

void
timer_start_nointr (enum timer_channel ch)
{

  __reg_write_4 (ch, T_MR0, ~0);  // Match count. 1 XXX XXX XXX
  __reg_write_4 (ch, T_TC, 0);
  __reg_write_4 (ch, T_TCR, TCR_ENABLE);
}

static inline void
__timer_start (enum timer_channel ch)
{
  uint32_t vic_bit = vic_bits[ch];

  *VICIntSelect &= ~vic_bit; // IRQ
  *VICIntEnable |= vic_bit;
  __reg_write_4 (ch, T_TC, 0);
  __reg_write_4 (ch, T_TCR, TCR_ENABLE);
}

static inline uint32_t
__timer_stop (enum timer_channel ch)
{
  uint32_t t;

  t = __reg_read_4 (ch, T_TC);
  __reg_write_4 (ch, T_TCR, 0);
  __reg_write_4 (ch, T_MCR, 0);
  *VICIntEnable &= ~vic_bits[ch];

  return t;
}
__END_DECLS

void
timer_init ()
{
  int i;

  // Enable all timer.
  for (i = 0; i < 4; i++)
    {
      mcu_peripheral_power (power_bits[i], TRUE);
      mcu_peripheral_clock (clock_bits[i], CCLK1);
    }
  //  timer_debug ();
}

void
__timer_config (enum timer_channel ch, int conf, enum timer_unit unit)
{
  // calcurate prescale.
  uint32_t prescale = mcu_cpuclock / unit;

  // Set prescale.
  __reg_write_4 (ch, T_PR, prescale);
  // Interrupt, Counter reset/stop.
  __reg_write_4 (ch, T_MCR, conf);
}

void
timer_fini ()
{

  __timer_stop (TIMER0);
  __timer_stop (TIMER1);
  __timer_stop (TIMER2);
  __timer_stop (TIMER3);
}

bool
md_timer_start (int channel, timer_counter_t interval,
		enum timer_counter_unit counter_unit,
		enum timer_counter_match counter_conf)
{
  enum timer_channel ch = (enum timer_channel)channel;
  int conf;

  if (md_timer_busy (channel))
    return FALSE;

  // Match count.
  __reg_write_4 (ch, T_MR0, interval);

  conf = TIMER_INTR_ENABLE;
  if (counter_conf == COUNTER_STOP)
    conf |= TIMER_COUNTER_STOP;
  else
    conf |= TIMER_COUNTER_RESET;

  if (counter_unit == COUNTER_USEC)
    __timer_config (ch, conf, TIMER_USEC);
  else
    __timer_config (ch, conf, TIMER_MSEC);

  //XXX XXX XXX
  if (channel < 2)
    __timer_start (ch);
  else
    timer_start_nointr (ch);
  //XXX XXX XXX

  return TRUE;
}

void
md_timer_stop (int channel)
{

  __timer_stop (channel);
}

bool
md_timer_busy (int channel)
{

  return __reg_read_4 ((enum timer_channel)channel, T_MCR) & TIMER_INTR_ENABLE;
}

void
timer0_intr ()
{

  *T0IR = 1;
  __timer_stop (TIMER0);
//  iprintf ("%s\n", __FUNCTION__);
  timer_do_schedule ();
}
#ifndef THREAD_DISABLE
void
timer1_intr ()
{

  *T1IR = 1;
  __timer_stop (TIMER1);
  timer_do_wakeup ();
}
#endif // !THREAD_DISABLE
