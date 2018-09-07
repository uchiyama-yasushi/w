
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

#ifndef _LPC23XX_UART_H_
#define	_LPC23XX_UART_H_
#define	UART0_BASE	0xe000c000
#define	UART1_BASE	0xe0010000
#define	UART2_BASE	0xe0078000
#define	UART3_BASE	0xe007c000

#define	_ULCR	0x0c
#define	_UDLL	0x00
#define	_UDLM	0x04
#define	_UFDR	0x28
#define	_UIER	0x04
#define	_ULSR	0x14
#define	_UTHR	0x00
#define	_URBR	0x00
#define	ULCR(a)		((volatile uint32_t *)((a) + _ULCR))
#define	UDLL(a)		((volatile uint32_t *)((a) + _UDLL))
#define	UDLM(a)		((volatile uint32_t *)((a) + _UDLM))
#define	UFDR(a)		((volatile uint32_t *)((a) + _UFDR))
#define	UIER(a)		((volatile uint32_t *)((a) + _UIER))
#define	ULSR(a)		((volatile uint32_t *)((a) + _ULSR))
#define	UTHR(a)		((volatile uint32_t *)((a) + _UTHR))
#define	URBR(a)		((volatile uint32_t *)((a) + _URBR))

// Line Control Register
#define	U0LCR	((volatile uint32_t *)0xe000c00c)
#define	 ULCR_DLAB		0x80	//Divisor Latch Access Bit
#define	 ULCR_BREAK_ENABLE	0x40
#define	 ULCR_BREAK_DISABLE	0x00
#define	 ULCR_PODD		0x00
#define	 ULCR_PEVEN		0x10
#define	 ULCR_P1		0x20
#define	 ULCR_P0		0x30
#define	 ULCR_PARITY		0x08
#define	 ULCR_PARITY_NONE	0x00
#define	 ULCR_STOP2		0x04
#define	 ULCR_STOP1		0x00
#define	 ULCR_5BIT		0x00
#define	 ULCR_6BIT		0x01
#define	 ULCR_7BIT		0x02
#define	 ULCR_8BIT		0x03

// Divisor Latch LSB
#define	U0DLL	((volatile uint32_t *)0xe000c000)
// Divisor Latch MSB
#define	U0DLM	((volatile uint32_t *)0xe000c004)
// Fractional Divider Register
#define	U0FDR	((volatile uint32_t *)0xe000c028)
// Interrupt Enable Register
#define	U0IER	((volatile uint32_t *)0xe000c004)
// Line Status Register
#define	U0LSR	((volatile uint32_t *)0xe000c014)
// Transmit Holding Register
#define	U0THR	((volatile uint32_t *)0xe000c000)
// Receiver Buffer Register
#define	U0RBR	((volatile uint32_t *)0xe000c000)

__BEGIN_DECLS
struct uart_clock_conf
{
  uint32_t DLM, DLL, FDR;
  enum pclk_divide cclk_divider;
};
// uart_clock_conf.c automatically generates parameters.
// Sample.
// { 0, 78, 1 | (2 << 4), 0 }; // PCLK 18.0000MHz 9615bps error 0.16%
// { 0, 39, 1 | (2 << 4), 0 }; // PCLK 18.0000MHz 19231bps error 0.16%
// { 0, 19, 7 | (13 << 4), 0 }; // PCLK 18.0000MHz 38487bps error 0.23%
// { 0, 13, 1 | (2 << 4), 0 }; // PCLK 18.0000MHz 57692bps error 0.16%
// { 0, 6, 5 | (8 << 4), 0 }; // PCLK 18.0000MHz 115385bps error 0.16%
// { 0, 3, 5 | (8 << 4), 0 }; // PCLK 18.0000MHz 230769bps error 0.16%
enum uart_channel
  {
    UART0 = 0,
    UART1 = 1,
    UART2 = 2,
    UART3 = 3,
  };

void uart_init (enum uart_channel, struct uart_clock_conf *, bool);
void uart_putc (enum uart_channel, int8_t);
int8_t uart_getc (enum uart_channel);

__END_DECLS

#endif
