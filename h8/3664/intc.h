
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

#ifndef _INTC_H_
#define	_INTC_H_

/* IRQ EdGe select Register */
#define	IEGR1	((volatile uint8_t *)0xfff2)
#define	 IEGR1_NMIEG	0x80
#define	 IEGR1_IEG3	0x08
#define	 IEGR1_IEG2	0x04
#define	 IEGR1_IEG1	0x02
#define	 IEGR1_IEG0	0x01

#define	IEGR2	((volatile uint8_t *)0xfff3)
/* IRQ ENable Register */
#define	IENR1	((volatile uint8_t *)0xfff4)
#define	 IENR1_IENDT	0x80
#define	 IENR1_IENTA	0x40
#define	 IENR1_IENWP	0x20
#define	 IENR1_IEN3	0x08
#define	 IENR1_IEN2	0x04
#define	 IENR1_IEN1	0x02
#define	 IENR1_IEN0	0x01


/* IRQ Flag Register */
#define	IRR1	((volatile uint8_t *)0xfff6)
#define	 IRR1_IRRDT	0x80
#define	 IRR1_IRRTA	0x40
#define	 IRR1_IRRI3	0x08
#define	 IRR1_IRRI2	0x04
#define	 IRR1_IRRI1	0x02
#define	 IRR1_IRRI0	0x01


/* WakeuP Interrupt Register */
#define	IWPR	((volatile uint8_t *)0xfff8)

#define	VECTOR_MIN		7	//NMI
#define	VECTOR_MAX		25
#endif



