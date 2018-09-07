
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

// 8bit timers (TMR)

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>
#include <clock.h>

#define	TMR_CHANNEL_MAX	8

STATIC uint8_t tmr_power_flag;
STATIC uint8_t tmr_intr_flag;

#define	TMR_UNIT_POWER(ch)	((tmr_power_flag >> (((ch) >> 1) << 1)) & 0x3)
#define	TMR_INTPRI_0_3(ch)	(IPRH_ADDR | ((3 - (ch)) << 2) << IPRSHIFT_SHIFT)
#define	TMR_CHANNEL_TO_UNIT(ch)	((ch) >> 1)

STATIC addr_t tmr_base[] =
  { _TMR0_BASE, _TMR1_BASE, _TMR2_BASE, _TMR3_BASE,
    _TMR4_BASE, _TMR5_BASE, _TMR6_BASE, _TMR7_BASE };

#define	tmr_reg(n, r)	((volatile uint8_t *)tmr_base[n] + _TMR_ ## r)

STATIC enum module_power tmr_power[] =
  { PWR_TMR_0_1, PWR_TMR_2_3, PWR_TMR_4_5, PWR_TMR_6_7 };

void
tmr_init (enum tmr_channel channel, enum interrupt_priority pri)
{
  uint8_t chbit = 1 << channel;

  cpu_status_t s = intr_suspend ();

  // Power on
  if (!TMR_UNIT_POWER (channel))
    peripheral_power (tmr_power [TMR_CHANNEL_TO_UNIT (channel)], TRUE);

  // Interrupt
  if (channel < 4)
    {
      intc_priority (TMR_INTPRI_0_3 (channel), pri);
    }
  else
    {
      // Override interrupt priority channel 4-7
      intc_priority (INTPRI_TMR_4_5_6_7, pri);
    }
  *tmr_reg (channel, TCR) = 0;	// Stop clock.

  tmr_power_flag |= chbit;
  tmr_intr_flag |= chbit;

  intr_resume (s);
}

bool
tmr_active (enum tmr_channel channel)
{

  return tmr_power_flag & (1 << channel);
}

void
tmr_fini (enum tmr_channel channel)
{
  uint8_t chbit = 1 << channel;

  cpu_status_t s = intr_suspend ();

  tmr_power_flag &= ~chbit;
  tmr_intr_flag &= ~chbit;

  // Check the other channel of this unit is active.
  if (!TMR_UNIT_POWER (channel))
    peripheral_power (tmr_power [TMR_CHANNEL_TO_UNIT (channel)], FALSE);

  if (channel < 4)
    {
      intc_priority (TMR_INTPRI_0_3 (channel), INTPRI_0);
    }
  else if (!(tmr_intr_flag & 0xf0))
    {
      intc_priority (INTPRI_TMR_4_5_6_7, INTPRI_0);
    }

  intr_resume (s);
}

void
tmr_start (enum tmr_channel channel, struct tmr_config *config)
{

  *tmr_reg (channel, TCORA) = config->TCORA;
  *tmr_reg (channel, TCORB) = config->TCORB;
  *tmr_reg (channel, TCCR) = config->TCCR;
  *tmr_reg (channel, TCNT) = 0;
  // Start timer.
  *tmr_reg (channel, TCR) = config->TCR;
}

uint8_t
tmr_stop (enum tmr_channel channel)
{
  // Stop timer.
  *tmr_reg (channel, TCR) = TMR_CKS_NOCLK;
  // Return last count.
  return *tmr_reg (channel, TCNT);
}

bool
tmr_busy (enum tmr_channel channel)
{

  return *tmr_reg (channel, TCR) & TCR_CKS_MASK;
}

uint8_t
tmr_constant_calc (struct tmr_config *config, uint32_t usec)
{
  static struct
  {
    int val;
    uint8_t CKS;
    uint8_t ICKS;
  } div[] = {
    { 2, TMR_CKS_INTCLK_2_8, TMR_ICKS_div2_RE },
    { 8, TMR_CKS_INTCLK_2_8, TMR_ICKS_div8_RE },
    { 32, TMR_CKS_INTCLK_32_64, TMR_ICKS_div32_RE },
    { 64, TMR_CKS_INTCLK_32_64, TMR_ICKS_div64_RE },
    { 1024, TMR_CKS_INTCLK_1024_8192, TMR_ICKS_div1024_RE },
    { 8192, TMR_CKS_INTCLK_1024_8192, TMR_ICKS_div8192_RE },
  };

  uint32_t pck = clock_peripheral () / 1000000;// MHz
  uint32_t cnt;
  size_t i;

  for (i = 0; i < sizeof div / sizeof div[0]; i++)
    if ((cnt = (pck * usec) / div[i].val) < 0xff)
      break;

  if (i == sizeof div / sizeof div[0])
    return 0;

  config->TCCR &= ~TCCR_ICKS_MASK;
  config->TCCR |= div[i].ICKS;
  config->TCR &= ~TCR_CKS_MASK;
  config->TCR |= div[i].CKS;

  return cnt;
}
