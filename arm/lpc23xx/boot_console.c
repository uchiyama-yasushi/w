
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

// LPC23XX boot console. use UART0

#include <sys/system.h>
#include <sys/console.h>
#include <console_machdep.h>

#include <reg.h>

BOOT_CONSOLE_DECL (uart0);
STATIC void uart0_init (struct uart_clock_conf *conf);

void
boot_console_init (struct uart_clock_conf *conf)
{
  if (conf)
    uart0_init (conf);

  _boot_console_init (&uart0_console_ops);
}

void
uart0_init (struct uart_clock_conf *conf)
{
  // Pin select
  mcu_pin_select (0, 2, 1); // Port0.2 : TXD0
  mcu_pin_select (0, 3, 1); // Port0.3 : RXD0

  // Clock supply
  mcu_peripheral_clock (PCLK_UART0, conf->cclk_divider);

  // Power supply
  mcu_peripheral_power (PCONP_PCUART0, TRUE);

  // Set speed.
  *U0LCR = ULCR_DLAB;	// Enable Divisor Latche access.
  *U0DLM = conf->DLM;
  *U0DLL = conf->DLL;
  *U0FDR = conf->FDR;

  // Set line format. 8N1.
  *U0LCR = ULCR_8BIT | ULCR_PARITY_NONE | ULCR_STOP1 | ULCR_BREAK_DISABLE;
  *U0IER = 0;	// Interrupt disable.
}

void
uart0_putc (void *arg __attribute__((unused)), int8_t c)
{

  if (c == '\n')
    md_uart_putc1 ('\r');
  md_uart_putc1 (c);
}

int8_t
uart0_getc (void *arg)
{
  struct uart0_boot_console *cons = (struct uart0_boot_console *)arg;
  uint8_t c;

  if ((c = cons->save))
    {
      cons->save = 0;
      return c;
    }

  while ((*U0LSR & 0x01) == 0)
    ;
  c = *U0RBR;

  return c;
}

int8_t
uart0_getc_cont (void *arg, continuation_func_t cont __attribute__((unused)))
{

  return uart0_getc (arg);
}

void
md_uart_putc1 (int8_t c)
{

  while ((*U0LSR & 0x20) == 0)
    ;
  *U0THR = c;
}

int8_t
uart0_ungetc (void *arg, int8_t c)
{
  struct uart0_boot_console *cons = (struct uart0_boot_console *)arg;

  if (c == EOF)
    return EOF;

  cons->save = c;

  return c;
}
