
/*-
 * Copyright (c) 2009, 2011 UCHIYAMA Yasushi.  All rights reserved.
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

BOOT_CONSOLE_DECL (sci1);

void
boot_console_init (bool on)
{
  on = on;

  *SCSCR1 = 0x2;	/*CKE1 external clock */
  *SCSCR1 |= (1 << 5)/*TE*/ | (1 << 4)/*RE*/;
  *SCSMR1 = 1/*CKS0*/;
  *SCBRR1 = 0;
  *SCFCR1 = 0;
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

  while (((c = *SCFSR1) & 0x2/*RDF*/) == 0)
    ;

  c = *SCFRDR1;
  *SCFSR1 &= ~0x2/*RDF*/;

  return c;
}

int8_t
sci1_getc_cont (void *arg, continuation_func_t cont __attribute__((unused)))
{

  return sci1_getc (arg);
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

void
md_uart_putc1 (int8_t c)
{

  while ((*SCFSR1 & (1 << 5)/*TDFE*/) == 0)
    ;
  *SCFTDR1 = c;
  *SCFSR1 &= ~((1 << 5)/*TDFE*/ | (1 << 6)/*TEND*/);
  while ((*SCFSR1 & (1 << 6)/*TEND*/) == 0)
    ;
}
