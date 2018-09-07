
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

// H8/3664

// Pull Up Control Register
#define	PUCR1	((volatile uint8_t *)0xffd0)
#define	PUCR5	((volatile uint8_t *)0xffd1)

// Data Register
#define	PDR1	((volatile uint8_t *)0xffd4)
#define	PDR2	((volatile uint8_t *)0xffd5)
#define	PDR5	((volatile uint8_t *)0xffd8)
#define	PDR7	((volatile uint8_t *)0xffda)
#define	PDR8	((volatile uint8_t *)0xffdb)
#define	PDRB	((volatile uint8_t *)0xffdd)

// Mode Register
#define	PMR1	((volatile uint8_t *)0xffe0)	// Port 1, 2
#define	 PMR1_IRQ3	0x80	//P17
#define	 PMR1_IRQ2	0x40	//P16
#define	 PMR1_IRQ1	0x20	//P15
#define	 PMR1_IRQ0	0x10	//P14
#define	 PMR1_TXD	0x02	//P22 (Port 2)
#define	 PMR1_TMOW	0x01	//P10
#define	PMR5	((volatile uint8_t *)0xffe1)	// Port 5
#define	 PMR5_WPK5	0x20
#define	 PMR5_WPK4	0x10
#define	 PMR5_WPK3	0x08
#define	 PMR5_WPK2	0x04
#define	 PMR5_WPK1	0x02
#define	 PMR5_WPK0	0x01

// Control Regsiter.
#define	PCR1	((volatile uint8_t *)0xffe4)
#define	PCR2	((volatile uint8_t *)0xffe5)
#define	PCR5	((volatile uint8_t *)0xffe8)
#define	PCR7	((volatile uint8_t *)0xffea)
#define	PCR8	((volatile uint8_t *)0xffeb)
