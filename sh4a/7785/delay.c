
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
#include <sys/delay.h>
#include <sys/console.h>
#include <reg.h>

uint32_t udelay_param;

void
delay_init ()
{

  udelay_param = cpg_pck () / 1000000 / 4;
}

void
delay_calibrate ()
{

  printf ("udelay_param = %d\n", udelay_param);
}

void
mdelay (uint32_t n)
{

  while (n--)
    udelay (1000); //1msec
}

void
udelay (uint16_t n)
{
  cpu_status_t s = intr_suspend ();

  *TSTR1 &= ~TSTR_STR5;	// Stop TMU5
  *TCNT5 = n * udelay_param;
  *TCOR5 = 0;
  *TCR5 = TPSC_PCK_4;
  *TSTR1 |= TSTR_STR5;	// Start TMU5
  while ((*TCR5 & TCR_UNF) == 0)
    ;
  *TSTR1 &= ~TSTR_STR5;	// Stop TMU5
  intr_resume (s);
}
