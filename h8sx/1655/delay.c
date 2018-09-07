
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

// MI delay interface.

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>
#include <sys/delay.h>
#include <sys/timer.h>
#ifdef DEBUG
#include <frame.h>
#endif

#ifdef DELAY_DEBUG
#define	DPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

extern uint16_t udelay_param;

uint8_t __delay_calibrate (struct tmr_config *, int);

void
delay_init ()
{
  cpu_status_t s = intr_suspend ();
  bool active;
  uint8_t overhead;
  uint8_t cnt_1usec;
  int hint;
  struct tmr_config config;

  if (!(active = tmr_active (TMR_CH7)))
    {
      tmr_init (TMR_CH7, INTPRI_0);	// No interrupt.
    }
  if (tmr_busy (TMR_CH7))
    {
      os_panic ("TMR7 busy.");
    }

  cnt_1usec = tmr_constant_calc (&config, 1);
  config.TCORA = 0xff;	// Don't compare match.
  config.TCORB = 0xff;	// Don't compare match.
  config.TCR |= TCR_CCLR_NO;

  if ((overhead = __delay_calibrate (&config, 1)) == 0)
    {
      os_panic ("Can't measure delay overhead.");
    }

  DPRINTF ("1sec cnt = %d\n", cnt_1usec);
  for (hint = 2; (__delay_calibrate (&config, hint) - overhead) < cnt_1usec;
       hint++)
    ;

  DPRINTF ("estimated count=%d\n", hint);

  if (active)
    {
      tmr_fini (TMR_CH7);
    }

  intr_resume (s);
}

uint8_t
__delay_calibrate (struct tmr_config *config, int hint)
{
  uint8_t cnt_total;

  udelay_param = hint;

  tmr_start (TMR_CH7, config);
  udelay (1);
  cnt_total = tmr_stop (TMR_CH7);

  DPRINTF ("hint=%d total = %d\n", hint, cnt_total);

  return cnt_total;
}

void
mdelay (uint32_t n)
{

  while (n--)
    udelay (1000); //1msec
}
