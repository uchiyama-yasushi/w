
/*-
 * Copyright (c) 2008-2009, 2011 UCHIYAMA Yasushi.  All rights reserved.
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

#include <sys/delay.h>

#include <sys/console.h>
#include <console_machdep.h>

// for interrupt context or no threading support.
BOOT_CONSOLE_DECL (sci);

STATIC void sci_init (uint8_t);

#ifndef SCI_SPEED_BRR
//#define	SCI_SPEED_BRR	12	// 38400bps @ 16MHz
//#define	SCI_SPEED_BRR	3	// 115200bps @ 16MHz !CANT COMMUNICATE!
//#define	SCI_SPEED_BRR	4	// 115200bps @ 16MHz !CANT COMMUNICATE!
#if SYSTEM_CLOCK == 16
#define	SCI_SPEED_BRR	8	// 57600bps @ 16MHz
#elif SYSTEM_CLOCK == 20
#define	SCI_SPEED_BRR	10	// 57600bps @ 20MHz
#else
#error
#endif
#endif

void
boot_console_init (bool on)
{
  on = on;

  sci_init (SCI_SPEED_BRR);
  _boot_console_init (&sci_console_ops);
}

void
sci_init (uint8_t brr)
{

  //  console for booting. don't buffered.
  *PMR1 |= PMR1_TXD;	// Enable TXD pin (RXD -> SCR3.RE)

  *SCI_SCR3 = 0;	/* CKS1=0 CKS=0 : 1/1 clock (n = 0)*/
  *SCI_SMR = 0;	/* 8N1 */
  *SCI_BRR = brr;
  mdelay (1);		/* wait at least 1bit cycle */
  *SCI_SCR3 |= (SCR_TE | SCR_RE);// Transmit Enable/Receive Enable.
}


// Simple polling routines.
int8_t
sci_getc (void *arg)
{
  struct sci_boot_console *cons = (struct sci_boot_console *)arg;
  uint8_t c;

  if ((c = cons->save))
    {
      cons->save = 0;
      return c;
    }

  while (((c = *SCI_SSR) & (SSR_RDRF | SSR_ERR_BITS)) == 0)
    ;
  if (c & SSR_ERR_BITS)
    {
      *SCI_SSR &= ~(SSR_RDRF | SSR_ERR_BITS);
      return 0;
    }

  c = *SCI_RDR;
  // SCI3 automatically clear SSR_RDRF bit.

  return c;
}

int8_t
sci_getc_cont (void *arg, continuation_func_t cont __attribute__((unused)))
{

  return sci_getc (arg);
}

void
sci_putc (void *arg __attribute__((unused)), int8_t c)
{

  if (c == '\n')
    md_uart_putc1 ('\r');
  md_uart_putc1 (c);
}

void
md_uart_putc1 (int8_t c)
{

  while ((*SCI_SSR & SSR_TDRE) == 0)
    ;
  *SCI_TDR = c;
  // SCI3 automatically clear SSR_TDRE bit.
}

int8_t
sci_ungetc (void *arg, int8_t c)
{
#if 0
  arg=arg,c=c;return EOF;
#else
  struct sci_boot_console *cons = (struct sci_boot_console *)arg;

  if (c == EOF)
    return EOF;

  cons->save = c;

  return c;
#endif
}
