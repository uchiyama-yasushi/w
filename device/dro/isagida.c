
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
#include <string.h>
#include <sys/thread.h>

#include <sys/console.h>
#include <reg.h>
#include <sys/delay.h>

#include "caliper_manager.h"
#include "caliper_common.h"
#include "isagida.h"

#define	ISAGIDA_SIGN		0x01
#define	 ISAGIDA_SIGN_MINUS 	 0x01
#define	 ISAGIDA_SIGN_PLUS 	 0x00
#define	ISAGIDA_UNIT		0x04
#define	 ISAGIDA_UNIT_MM	 0x04
#define	 ISAGIDA_UNIT_INCH	 0x00
#define	ISAGIDA_INCH_HALF	0x02

__BEGIN_DECLS
STATIC bool isagida_update (caliper_t);
STATIC void isagida_unit_change (caliper_t);
//STATIC void isagida_bcd_preformat (char *, int8_t *);
STATIC int32_t isagida_data_format (uint8_t *);
__END_DECLS

caliper_t
isagida_attach (struct isagida_caliper *ic, int axis __attribute__((unused)),
		struct caliper_bits *bits)
{
  caliper_t c = &ic->caliper;

  c->ops.init = shan_init;
  c->ops.update = isagida_update;
  c->ops.power = shan_command_power;
  c->ops.fast = shan_command_fast;
  c->ops.reset = shan_command_reset;
  c->bits = *bits;

  memset (ic->bcd, 0, 8);
  ic->read_error = TRUE;

  return c;
}

bool
isagida_update (caliper_t caliper)
{
  struct isagida_caliper *ic = (struct isagida_caliper *)caliper;
  static int unit_change_delay;
  uint8_t bcd[8];

  if (!ic->caliper.power)
    return FALSE;

  cpu_status_t s = intr_suspend ();
  if (ic->read_error)
    {
      intr_resume (s);
      return FALSE;
    }
  memcpy (bcd, ic->bcd, 8);
  intr_resume (s);

  if ((bcd[6] & ISAGIDA_UNIT) == ISAGIDA_UNIT_MM)
    {
      caliper->raw_data = isagida_data_format (bcd);
    }
  else
    {
      if (--unit_change_delay < 0)
	{
	  s = intr_suspend ();
	  isagida_unit_change (caliper); // Change to millimeter.
	  ic->read_error = TRUE;
	  intr_resume (s);
	  unit_change_delay = 50;
	}
    }

  return TRUE;
}

// caliper specific preformat.
int32_t
isagida_data_format (uint8_t *d)
{
  int i;
  int32_t a, j;

  for (i = 0, j = 1, a = 0; i < 5; i++, j *= 10)
    a += (int32_t)d[i] * j;
  if (d[6] & ISAGIDA_SIGN)
    a = -a;

  return a;
}

void
isagida_unit_change (caliper_t caliper)
{
  cpu_status_t s = intr_suspend ();

  *caliper->power_port |= caliper->bits.power;	// Caliper POWER off
  mdelay (400);
  *caliper->power_port &= ~caliper->bits.power;	// Caliper POWER on
  mdelay (400);
  intr_resume (s);

  iprintf ("%s\n", __FUNCTION__);
}

#if 0
// direct conversion. not used now.
void
isagida_bcd_preformat (char *scratch, int8_t *bcd)
{
  int i;
  int start;

  memset (scratch, BLANK_CHARACTER, 8);
  scratch[7] = '\0';
  for (i = 0; i < 3; i++)
    {
      scratch[6 - i] = '0' + bcd[i];
    }
  start = FALSE;
  for (i = 5; i >=3; i--)
    {
      if (!start && bcd[i])
	{
	  if (bcd[7])
	    scratch[5 - i] = '-';
	  start = TRUE;
	}
      if (start)
	scratch[6 - i] = '0' + bcd[i];
    }
  if (!start && bcd[7])
    scratch[3] = '-';
}
#endif
