
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

#ifndef _LPC23XX_GPIO_H_
#define	_LPC23XX_GPIO_H_
//
// Port0 and Port2 provide an interrupt for each port pin.
//
#define	FIODIR		0x00
#define	FIOMASK		0x10
#define	FIOPIN		0x14
#define	FIOSET		0x18
#define	FIOCLR		0x1c

#define	FIO0Base	0x3fffc000
#define	FIO1Base	0x3fffc020
#define	FIO2Base	0x3fffc040
#define	FIO3Base	0x3fffc060
#define	FIO4Base	0x3fffc080

#define	FIO0DIR		((volatile uint32_t *)0x3fffc000)
#define	FIO0MASK	((volatile uint32_t *)0x3fffc010)
#define	FIO0PIN		((volatile uint32_t *)0x3fffc014)
#define	FIO0SET		((volatile uint32_t *)0x3fffc018)
#define	FIO0CLR		((volatile uint32_t *)0x3fffc01c)

#define	FIO1DIR		((volatile uint32_t *)0x3fffc020)
#define	FIO1MASK	((volatile uint32_t *)0x3fffc030)
#define	FIO1PIN		((volatile uint32_t *)0x3fffc034)
#define	FIO1SET		((volatile uint32_t *)0x3fffc038)
#define	FIO1CLR		((volatile uint32_t *)0x3fffc03c)

#define	FIO2DIR		((volatile uint32_t *)0x3fffc040)
#define	FIO2MASK	((volatile uint32_t *)0x3fffc050)
#define	FIO2PIN		((volatile uint32_t *)0x3fffc054)
#define	FIO2SET		((volatile uint32_t *)0x3fffc058)
#define	FIO2CLR		((volatile uint32_t *)0x3fffc05c)

#define	FIO3DIR		((volatile uint32_t *)0x3fffc060)
#define	FIO3MASK	((volatile uint32_t *)0x3fffc070)
#define	FIO3PIN		((volatile uint32_t *)0x3fffc074)
#define	FIO3SET		((volatile uint32_t *)0x3fffc078)
#define	FIO3CLR		((volatile uint32_t *)0x3fffc07c)

#define	FIO4DIR		((volatile uint32_t *)0x3fffc080)
#define	FIO4MASK	((volatile uint32_t *)0x3fffc090)
#define	FIO4PIN		((volatile uint32_t *)0x3fffc094)
#define	FIO4SET		((volatile uint32_t *)0x3fffc098)
#define	FIO4CLR		((volatile uint32_t *)0x3fffc09c)

// GPIO interrupt.
#define	IO0IntBase	0xe0028084
#define	IO2IntBase	0xe00280a4
#define	IntEnR		0x0c
#define	IntEnF		0x10
#define	IntStatR	0x00
#define	IntStatF	0x04
#define	IntClr		0x08

#define	IO0IntEnR	((volatile uint32_t *)0xe0028090)
#define	IO0IntEnF	((volatile uint32_t *)0xe0028094)
#define	IO0IntStatR	((volatile uint32_t *)0xe0028084)
#define	IO0IntStatF	((volatile uint32_t *)0xe0028088)
#define	IO0IntClr	((volatile uint32_t *)0xe002808c)

#define	IO2IntEnR	((volatile uint32_t *)0xe00280b0)
#define	IO2IntEnF	((volatile uint32_t *)0xe00280b4)
#define	IO2IntStatR	((volatile uint32_t *)0xe00280a4)
#define	IO2IntStatF	((volatile uint32_t *)0xe00280a8)
#define	IO2IntClr	((volatile uint32_t *)0xe00280ac)

#define	IOIntStat	((volatile uint32_t *)0xe0028080)
#define	 IOIntStatPending0	0x1
#define	 IOIntStatPending2	0x4

#define	GPIO_PIN_CLR(port, pin)	*FIO ## port ## PIN &= ~(1 << pin)
#define	GPIO_PIN_SET(port, pin)	*FIO ## port ## PIN |= (1 << pin)

#define	GPIO_PIN(port, pin)	(*FIO ## port ## PIN & (1 << pin))

__BEGIN_DECLS
enum gpio_dir
{
  GPIO_INPUT,
  GPIO_OUTPUT,
};
#define	GPIO_EDGE_F	0x1
#define	GPIO_EDGE_R	0x2
void gpio_dir (int, int, enum gpio_dir);
void gpio_interrupt (int, int, int);
__END_DECLS

#endif
