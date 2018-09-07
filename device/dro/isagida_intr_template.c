
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
#include <sys/thread.h>

#include <sys/console.h>
#include <reg.h>
#include <sys/delay.h>

#include "isagida.h"

__BEGIN_DECLS
void FUNC_NAME (void);
extern caliper_t __caliper[];
__END_DECLS

void
FUNC_NAME ()
{
  struct isagida_caliper *ic = (struct isagida_caliper *)__caliper[CALIPER_AXIS];
  int i, j, k;
  uint8_t *bcd = ic->bcd;

  ic->read_error = TRUE;
  for (i = 0; i < 7; i++)
    {
      bcd[i] = 0;
      for (j = 0; j < 4; j++)
	{
	  for (k = 0; !(*CLK_PORT & CLK_BIT) && k < 100; k++) // LOW
	    ;
	  if (!(*DATA_PORT & DATA_BIT))
	    bcd[i] |= (1 << j);
	  if (k == 100)
	    {
	      goto next_phase;
	    }
	  //HIGH
	  for (k = 0; (*CLK_PORT & CLK_BIT) && k < 100; k++) // HIGH
	    ;
	  if (k == 100)
	    {
	      goto next_phase;
	    }
	}
    }
  ic->read_error = FALSE;
  thread_wakeup (caliper_manager_thread);
 next_phase:
  // Drain unneeded interrupt.
  *INTC_ISR &= ~ISR_BIT;
}
