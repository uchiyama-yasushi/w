
/*-
 * Copyright (c) 2010-2011 UCHIYAMA Yasushi.  All rights reserved.
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
#include <reg.h>

#include <sys/console.h>
#include <console_machdep.h>

#include <stdio.h>

BOOT_CONSOLE_DECL (sci4);

STATIC void sci4_init (uint8_t);

void
boot_console_init (bool on, uint8_t brr)
{
  on = on;
  sci4_init (brr);
  _boot_console_init (&sci4_console_ops);
}

void
sci4_init (uint8_t brr)
{
  // Start SCI4 module
  peripheral_power (PWR_SCI4, TRUE);

  // Disable transmit/receive.
  *SCI (4, SCR) &= ~(SCR_TE | SCR_RE);

  // Setup Input buffer. Use internal clock.
  *P6_ICR &= ~1;	//P60: SCI4 TxD
  *P6_ICR |= 2;		//P61: SCI4 RxD

  // Clock select
  *SCI (4, SCR) &= ~(SCR_CKE0 | SCR_CKE1); // Peripheral clock / 1

  // Tx/Rx Format.
  *SCI (4, SCMR) = 0;	// Clocked synchronous mode.
  *SCI (4, SMR)	= 0;	// 8N1

  // Bit rate. CKS=0, CKS=1, ABCS=0 (ABCS bit:SCI2,5,6 only)
  /*
     Peripheral clock 12MHz
     38:     9600->38.062 (0.16%)
     9:    38400->8.766 (-2.34%)
     6:    57600->5.510 (-6.99%)
     2:   115200->2.255 (8.51%)
     1:   230400->0.628 (-18.62%)
  */
  /*
     Peripheral clock 24MHz
     77:     9600->77.125 (0.16%)	OK
     19:    38400->18.531 (-2.34%)	OK
     12:    57600->12.021 (0.16%)	OK
     6:   115200->5.510 (-6.99%)	NG
     2:   230400->2.255 (8.51%)		NG
  */

  *SCI (4, BRR) = brr;

  // Wait at least 1bit interval.
  volatile int i;
  for (i = 0; i < 0xffff; i++)
    ;

  // Enable transmit/receive.
  *SCI (4, SCR) |= (SCR_TE | SCR_RE);
}

// Simple polling routines.
void
md_uart_putc1 (int8_t c)
{

  while ((*SCI (4, SSR) & SSR_TDRE) == 0)
    ;
  *SCI (4, TDR) = c;
  *SCI (4, SSR) &= ~SSR_TDRE;

  // Wait transfer complete.
  while ((*SCI (4, SSR) & SSR_TEND))
    ;
}

int8_t
sci4_getc (void *arg)
{
  struct sci4_boot_console *cons = (struct sci4_boot_console *)arg;
  char c;

  if ((c = cons->save))
    {
      cons->save = 0;
      return c;
    }

  while (((c = *SCI (4, SSR)) & (SSR_RDRF | SSR_ERR_BITS)) == 0)
    ;
  if (c & SSR_ERR_BITS)
    {
      *SCI (4, SSR) &= ~(SSR_RDRF | SSR_ERR_BITS);
      return 0;
    }

  c = *SCI (4, RDR);
  *SCI (4, SSR) &= ~SSR_RDRF;

  return c;
}

int8_t
sci4_getc_cont (void *arg, continuation_func_t cont __attribute__((unused)))
{

  return sci4_getc (arg);
}

void
sci4_putc (void *arg __attribute__((unused)), int8_t c)
{

  if (c == '\n')
    md_uart_putc1 ('\r');
  md_uart_putc1 (c);
}

int8_t
sci4_ungetc (void *arg, int8_t c)
{
  struct sci4_boot_console *cons = (struct sci4_boot_console *)arg;

  if (c == EOF)
    return EOF;

  cons->save = c;

  return c;
}
