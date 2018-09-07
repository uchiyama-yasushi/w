
/*-
 * Copyright (c) 2010 UCHIYAMA Yasushi.  All rights reserved.
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
// H8SX/1655 I/O port

#ifndef _H8SX_1655_IOPORT_H_
#define	_H8SX_1655_IOPORT_H_

/* Port Function Control Register */
#define	PFCR2		((volatile uint8_t *)0xfffbc2)
#define	PFCRC		((volatile uint8_t *)0xfffbcc)

/* Port5 (P56,P57: D/A converter) */
#define	P5_PORT		((volatile uint8_t *)0xffff44)
#define	P5_ICR		((volatile uint8_t *)0xfffb94)


/* Port 6 (SCI4) */
/* Input buffer Control Register  1:Enable, 0:Disable */
#define	P6_ICR		((volatile uint8_t *)0xfffb95)

/* Port A (BSC) */
#define	PA_ICR		((volatile uint8_t *)0xfffb99)
#define	PA_DDR		((volatile uint8_t *)0xfffb89)
#define	PA_PORT		((volatile uint8_t *)0xffff49)
#define	PA_DR		((volatile uint8_t *)0xffff59)

/* Port F (BSC, Address line) */
#define	PF_ICR		((volatile uint8_t *)0xfffb9e)
#define	PF_DDR		((volatile uint8_t *)0xfffb8e)
#define	PF_PCR		((volatile uint8_t *)0xfffbb6)
#define	PF_ODR		((volatile uint8_t *)0xfffbbd)
#define	PF_PORT		((volatile uint8_t *)0xffff4d)
#define	PF_DR		((volatile uint8_t *)0xffff5e)

/* Port M */
/*
  0	SCI6 TxD
  1	SCI6 RxD
  2
  3
  4	USB PULLUP_E
 */
#define	PM_DDR	((volatile uint8_t *)0xffee50)
#define	PM_DR	((volatile uint8_t *)0xffee51)
#define	PM_PORT	((volatile uint8_t *)0xffee52)
#define	PM_ICR	((volatile uint8_t *)0xffee53)

#define	BSET(bit, addr)	\
  __asm volatile ("bset %0l, @%1":: "r"((uint8_t)(bit)), "r"(addr))
#define	BCLR(bit, addr)	\
  __asm volatile ("bclr %0l, @%1":: "r"((uint8_t)(bit)), "r"(addr))

#endif
