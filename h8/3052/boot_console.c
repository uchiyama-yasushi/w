
/*-
 * Copyright (c) 2008, 2011 UCHIYAMA Yasushi.  All rights reserved.
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

BOOT_CONSOLE_DECL (sci1);

#ifndef SCI_SPEED_BRR
#define	SCI_SPEED_BRR	6	// 115200bps @ 25MHz
// 25*1000000/(64*2^(2*0 - 1)*115200) - 1 = 5.78
#endif
//#define	SCI_SPEED_BRR	13 57600bps @ 25MHz
// 25*1000000/(64*2^(2*0 - 1)*57600) - 1 = 12.56

void
boot_console_init (bool on)
{
  on = on;

  //  console for booting. don't buffered.
  *SCI1_SCR = 0;	/* CKS1=0 CKS=0 : 1/1 clock (n = 0)*/
  *SCI1_SMR = 0;	/* 8N1 */
  *SCI1_BRR = SCI_SPEED_BRR;
  mdelay (1);		/* wait at least 1bit cycle */
  /* 1bit cycle: 1/115200*1000000=8.68us */
  *SCI1_SCR |= (SCR_TE | SCR_RE);// Transmit Enable/Receive Enable.

  _boot_console_init (&sci1_console_ops);
}

// Simple polling routines.
void
sci1_putc (void *arg __attribute__((unused)), int8_t c)
{

  if (c == '\n')
    md_uart_putc1 ('\r');
  md_uart_putc1 (c);
}

int8_t
sci1_getc (void *arg)
{
  struct sci1_boot_console *cons = (struct sci1_boot_console *)arg;
  uint8_t c;

  if ((c = cons->save))
    {
      cons->save = 0;
      return c;
    }

  while (((c = *SCI1_SSR) & (SSR_RDRF | SSR_ERR_BITS)) == 0)
    ;
  if (c & SSR_ERR_BITS)
    {
      *SCI1_SSR &= ~(SSR_RDRF | SSR_ERR_BITS);
      return 0;
    }

  c = *SCI1_RDR;
  *SCI1_SSR &= ~SSR_RDRF;

  return c;
}

int8_t
sci1_getc_cont (void *arg, continuation_func_t cont __attribute__((unused)))
{

  return sci1_getc (arg);
}

void
md_uart_putc1 (int8_t c)
{

  while ((*SCI1_SSR & SSR_TDRE) == 0)
    ;
  *SCI1_TDR = c;
  *SCI1_SSR &= ~SSR_TDRE;
}

int8_t
sci1_ungetc (void *arg, int8_t c)
{
  struct sci1_boot_console *cons = (struct sci1_boot_console *)arg;

  if (c == EOF)
    return EOF;

  cons->save = c;

  return c;
}
