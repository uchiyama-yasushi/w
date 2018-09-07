
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

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>

#ifdef DEBUG
#define	DPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

STATIC const char *clock_ratio_print (uint8_t);
uint32_t clock_calc (uint8_t bits);
STATIC bool clock_change (struct clock_config *);
STATIC bool clock_change_prepare (uint16_t *, int, int);

struct clock_config __clock;

bool
clock_init (struct clock_config *conf)
{

  __clock = *conf;
  if (!clock_change (conf))
    return FALSE;

  uint16_t sckcr = *SCKCR;
  __clock.ick_hz = clock_calc (SCKCR_ICK (sckcr));
  __clock.pck_hz = clock_calc (SCKCR_PCK (sckcr));
  __clock.bck_hz = clock_calc (SCKCR_BCK (sckcr));

  return TRUE;
}

void
clock_info ()
{
  uint16_t sckcr = *SCKCR;

  printf ("PA7 clock output: %s\n", sckcr & SCKCR_PSTOP1 ? "Off" : "On");
  printf ("Input clock: %dHz, MDCLK=%d\n", __clock.input_clock, 
	  __clock.MDCLK);
  printf ("System clock: x%s (%dHz)\n", clock_ratio_print (SCKCR_ICK (sckcr)),
	  clock_cpu ());
  printf ("Peripheral clock: x%s (%dHz)\n",
	  clock_ratio_print (SCKCR_PCK (sckcr)), clock_peripheral ());
  printf ("External clock: x%s (%dHz)\n", clock_ratio_print (SCKCR_BCK (sckcr)),
	  clock_external_bus ());
}

const char *
clock_ratio_print (uint8_t bits)
{
  static const char *ratio[] = { "4", "2", "1", "1/2" };
  int idx = __clock.MDCLK ? bits + 1 : bits;

  return idx > 3 ? "prohibit" : ratio[idx];
}

uint32_t
clock_calc (uint8_t bits)
{
  int mult[] = { 4, 2, 1, 1 };
  int div[]  = { 1, 1, 1, 2 };
  int idx = __clock.MDCLK ? bits + 1 : bits;

  return idx > 3 ? 0 : __clock.input_clock * mult[idx] / div[idx];
}

bool
clock_change (struct clock_config *conf)
{
  uint16_t sckcr = *SCKCR;

  if (clock_change_prepare (&sckcr, conf->ick, SCKCR_ICK_SHIFT) &&
      clock_change_prepare (&sckcr, conf->pck, SCKCR_PCK_SHIFT) &&
      clock_change_prepare (&sckcr, conf->bck, SCKCR_BCK_SHIFT))
    {
      *SCKCR = sckcr;
      return TRUE;
    }

  return FALSE;
}

bool
clock_change_prepare (uint16_t *sckcr, int ck, int shift)
{

  if ((__clock.MDCLK == 1) && (--ck < 0))	// MD_CLK == 1 && select x4
    return FALSE;	// Invalid setting.

  *sckcr &= ~(SCKCR_CK_MASK << shift);
  *sckcr |= ck << shift;

  return TRUE;
}
