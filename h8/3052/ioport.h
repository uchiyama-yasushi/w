
/*-
 * Copyright (c) 2008 UCHIYAMA Yasushi.  All rights reserved.
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

#ifndef _IOPORT_H_
#define	_IOPORT_H_

/* [Port 1] 8bit I/O  | A0-A7 */
#define	P1_DDR	((volatile uint8_t *)0xfffc0)	// W
#define	P1_DR	((volatile uint8_t *)0xfffc2)	// R/W

/* [Port 2] 8bit I/O pullup | A8-A15 */
#define	P2_DDR	((volatile uint8_t *)0xfffc1)	// W
#define	P2_DR	((volatile uint8_t *)0xfffc3)	// R/W
#define	P2_PCR	((volatile uint8_t *)0xfffd8)	// R/W

/* [Port 3] 8bit I/O | D8-D15 */
#define	P3_DDR	((volatile uint8_t *)0xfffc4)	// W
#define	P3_DR	((volatile uint8_t *)0xfffc6)	// R/W

/* [Port 4] 8bit I/O pullup | D0-D7 */
#define	P4_DDR	((volatile uint8_t *)0xfffc5)	// W
#define	P4_DR	((volatile uint8_t *)0xfffc7)	// R/W
#define	P4_PCR	((volatile uint8_t *)0xfffda)	// R/W

/* [Port 5] 4bit I/O pullup | A16-A19 */
#define	P5_DDR	((volatile uint8_t *)0xfffc8)	// W
#define	P5_DR	((volatile uint8_t *)0xfffca)	// R/W
#define	P5_PCR	((volatile uint8_t *)0xfffdb)	// R/W

/* [Port 6] 7bit I/O | LWR#,HWR#,RD#,AS#,BACK#,BREQ#,WAIT# */
#define	P6_DDR	((volatile uint8_t *)0xfffc9)	// W
#define	P6_DR	((volatile uint8_t *)0xfffcb)	// R/W

/* [Port 7] 8bit I | A/D, D/A */
#define	P7_DR	((volatile uint8_t *)0xfffce)	// R/W

/* [Port 8] 5bit I/O | CS0#-CS3#,RFSH#,IRQ0#-IRQ3# */
/*
   0: I/O	IRQ0	schmitt trigger, max volt: Vcc * 0.7	| RFSH
   1: I		IRQ1	schmitt trigger, max volt: Vcc * 0.7	| CS3
   2: I		IRQ2	schmitt trigger, max volt: Vcc * 0.7	| CS2
   3: I		IRQ3						| CS1
   4: I/O							| CS0
 */
#define	P8_DDR	((volatile uint8_t *)0xfffcd)	// W
#define	P8_DR	((volatile uint8_t *)0xfffcf)	// R/W

/* [Port 9] 6bit I/O | SCI0,SCI1,IRQ#4,IRQ#5 */
#define	P9_DDR	((volatile uint8_t *)0xfffd0)	// W
#define	P9_DR	((volatile uint8_t *)0xfffd2)	// R/W

/* [Port A] 8bit I/O | TPC,ITU,CS4#-CS6#,DMAC */
/*
  mode7
		TPC	ITU			DMAC

  0:PA0(I/O)	TP0(O)	TCLKA(I)		TEND0(O)
  1:PA1(I/O)	TP1(O)	TCLKB(I)		TEND1(O)
  2:PA2(I/O)	TP2(O)	TIOCA0(I/O)TCLKC(I)
  3:PA3(I/O)	TP3(O)	TIOCB0(I/O)TCLKD(I)
  4:PA4(I/O)	TP4(O)	TIOCA1(I/O)
  5:PA5(I/O)	TP5(O)	TIOCB1(I/O)
  6:PA6(I/O)	TP6(O)	TIOCA2(I/O)
  7:PA7(I/O)	TP7(O)	TIOCB2(I/O)

 */
#define	PA_DDR	((volatile uint8_t *)0xfffd1)	// W
#define	PA_DR	((volatile uint8_t *)0xfffd3)	// R/W

/* [Port B] 8bit I/O | TPC,ITU,DMAC,A/D,CS7# */
#define	PB_DDR	((volatile uint8_t *)0xfffd4)	// W
#define	PB_DR	((volatile uint8_t *)0xfffd6)	// R/W

#endif //_IOPORT_H_
