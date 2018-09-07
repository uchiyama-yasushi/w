
/*-
 * Copyright (c) 2010, 2011 UCHIYAMA Yasushi.  All rights reserved.
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
#include <console_machdep.h>

#define	PGCR4	((volatile uint16_t *)0xfffe38c6)

#define	SCSMR_6		((volatile uint16_t *)0xfffeb000)
#define	SCBRR_6		((volatile uint8_t *)0xfffeb004)
#define	SCSCR_6		((volatile uint16_t *)0xfffeb008)
#define	SCFTDR_6	((volatile uint8_t *)0xfffeb00c)
#define	SCFSR_6		((volatile uint16_t *)0xfffeb010)
#define	SCFRDR_6	((volatile uint8_t *)0xfffeb014)
#define	SCFCR_6		((volatile uint16_t *)0xfffeb018)
#define	SCFDR_6		((volatile uint16_t *)0xfffeb01c)
#define	SCSPTR_6	((volatile uint16_t *)0xfffeb020)
#define	SCLSR_6		((volatile uint16_t *)0xfffeb024)
#define	SCEMR_6		((volatile uint16_t *)0xfffeb028)

#define	STBCR4		((volatile uint8_t *)0xfffe040c)

BOOT_CONSOLE_DECL (sci6);
STATIC void sci6_init (uint8_t);

void
boot_console_init (bool on, uint8_t brr)
{

  on = on;
  sci6_init (brr);
  _boot_console_init (&sci6_console_ops);
}

void
sci6_init (uint8_t brr)
{
  // SCI6 Power on
  // SCI6 Power on
  if (*STBCR4 & (1 << 1))
    {
      *STBCR4 &= ~(1 << 1);
      *STBCR4;	// Dummy read. Ensure power on.
    }
  else
    {
      // Already Powered.
      // Drain FIFO
      while (*SCFDR_6 & (0x1f << 8))
	;
      // Wait until exhausted. //XXX
      int i;
      for (i = 0; i < 1000000; i++) //XXX
	;
    }

  // SCI initialize
  *SCSCR_6 &= ~(0xf << 4); // Drop RIE, TIE, TE, RE
  *SCFCR_6 |= (0x3 << 1); // Reset FIFO
  *SCFSR_6 |= 0;	// Clear Status.
  *SCSCR_6 &= ~0x3;	// Internal clock SCK pin is input.
  *SCSMR_6 = 0;	// 8N1 Pck 1/1 (Pck=24MHz)
  *SCEMR_6 = 0;	// x16 base clock
  *SCBRR_6 = brr;
  *SCFCR_6 = 0;	// Tx FIFO = 8, Rx FIFO = 1

  // Pin configuration
  // TxD6
  *PGCR4 &= ~(0x7 << 8);
  *PGCR4 |= (0x4 << 8);

  // RxD6
  *PGCR4 &= ~(0x7 << 4);
  *PGCR4 |= (0x4 << 4);

  // Enable trasmit/recieve.
  *SCSCR_6 |= (0x3 << 4); // TE,RE
}


// Simple polling routines.
void
md_uart_putc1 (int8_t c)
{
#define	TDFE	(1 << 5)
#define	TEND	(1 << 6)

  // Wait transmit FIFO
  while (!(*SCFSR_6 & TDFE))
    ;
  *SCFTDR_6 = c;
  *SCFSR_6 &= ~TDFE;
  // Wait transfer complete.
  while (!(*SCFSR_6 & (TDFE | TEND)))
    ;
  *SCFSR_6 &= ~(TDFE | TEND);
}

int8_t
sci6_getc (void *arg)
{
  struct sci6_boot_console *cons = (struct sci6_boot_console *)arg;
  char c;

  if ((c = cons->save))
    {
      cons->save = 0;
      return c;
    }

  // SCFSR
#define	ER	(1 << 7)
#define	DR	(1 << 0)
#define	RDF	(1 << 1)
  // SCLSR
#define	ORER	(1 << 0)
  do
    {
      if ((*SCFSR_6 & (ER | DR)) | (*SCLSR_6 & ORER))
	{
	  // Clear error status
	  *SCFSR_6 &= ~(ER | DR);
	  *SCLSR_6 &= ~ORER;
	  return 0;
	}
    }
  while (!(*SCFSR_6 & RDF))
    ;
  c = *SCFRDR_6;
  int n = *SCFDR_6 & 0x1f;

  if (n == 0)
    *SCFSR_6 &= ~RDF;

  return c;
}

int8_t
sci6_getc_cont (void *arg, continuation_func_t cont __attribute__((unused)))
{

  return sci6_getc (arg);
}

void
sci6_putc (void *arg __attribute__((unused)), int8_t c)
{

  if (c == '\n')
    md_uart_putc1 ('\r');
  md_uart_putc1 (c);
}

int8_t
sci6_ungetc (void *arg, int8_t c)
{
  struct sci6_boot_console *cons = (struct sci6_boot_console *)arg;

  if (c == EOF)
    return EOF;

  cons->save = c;

  return c;
}
