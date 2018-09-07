
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

/* digital caliper common routines. */
// This module is H8 port configuration independent.

#include <sys/system.h>
#include <reg.h>
#include <sys/delay.h>

#include "caliper_manager.h"
#include "caliper_common.h"

STATIC void caliper_pullup (caliper_t, uint8_t);

void
shan_init (caliper_t caliper)
{

 // Pull down Data, Clock line.
  *caliper->pullup_port &= ~(caliper->bits.data | caliper->bits.clock);
  shan_command_power (caliper);
  *INTC_IER |= caliper->bits.irq;	// Interrupt enable.
}

void
shan_command_power (caliper_t caliper)
{

  cpu_status_t s = intr_suspend ();
  caliper->power ^= 1;
  if (caliper->power)
    {
      *caliper->power_port &= ~caliper->bits.power;	// Caliper POWER ON
      caliper->fast_mode = FALSE;
    }
  else
    {
      *caliper->power_port |= caliper->bits.power;	// Caliper POWER OFF
    }
  mdelay (400);
  intr_resume (s);
}

void
shan_command_reset (caliper_t caliper)
{

  if (!caliper->power)
    return;

  cpu_status_t s = intr_suspend ();
  if (caliper->fast_mode)
    {
      shan_command_fast (caliper);	// toggle off
      caliper_pullup (caliper, caliper->bits.clock);
      shan_command_fast (caliper);	// toggle on
    }
  else
    {
      caliper_pullup (caliper, caliper->bits.clock);
    }
  intr_resume (s);
}

void
shan_command_fast (caliper_t caliper)
{

  if (!caliper->power)
    return;

  cpu_status_t s = intr_suspend ();
  caliper->fast_mode ^= 1;
  if (caliper->fast_mode)
    {
      caliper_pullup (caliper, caliper->bits.data);
      caliper_pullup (caliper, caliper->bits.clock);
    }
  else
    {
      caliper_pullup (caliper, caliper->bits.data);
    }
  intr_resume (s);
}

// Pull up clock/data line.
void
caliper_pullup (caliper_t caliper, uint8_t bits)
{

  *caliper->pullup_port |= bits;
  mdelay (400);
  *caliper->pullup_port &= ~bits;
}
