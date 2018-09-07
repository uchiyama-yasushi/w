
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

// H8/3664 Timer A
// Timer A connect to 32.768KHz sub crystal.

// Timer A

#define	TMA	((volatile uint8_t *)0xffa6)
#define	_TMA7		0x80
#define	_TMA6		0x40
#define	_TMA5		0x20
#define	TMA_OUT_S32	0
#define	TMA_OUT_S16	_TMA5
#define	TMA_OUT_S8	_TMA6
#define	TMA_OUT_S4	(_TMA6 | _TMA5)
#define	TMA_OUT_W32	_TMA7
#define	TMA_OUT_W16	(_TMA7 | _TMA5)
#define	TMA_OUT_W8	(_TMA7 | _TMA6)
#define	TMA_OUT_W4	(_TMA7 | _TMA6 | _TMA5)
#define	TMA_RESERVED	0x10

#define	_TMA3		0x08
#define	TMA_CLOCK	_TMA3	// 32.768kHz
#define	TMA_INTERVAL	0	// internal clock.

#define	_TMA2		0x04
#define	_TMA1		0x02
#define	_TMA0		0x01
#define	TMA_CLK_8182	0
#define	TMA_CLK_4096	_TMA0
#define	TMA_CLK_2048	_TMA1
#define	TMA_CLK_512	(_TMA1 | _TMA0)
#define	TMA_CLK_256	_TMA2
#define	TMA_CLK_128	(_TMA2 | _TMA0)
#define	TMA_CLK_32	(_TMA2 | _TMA1)
#define	TMA_CLK_8	(_TMA2 | _TMA1 | _TMA0)

#define	TMA_OVF_1000	0
#define	TMA_OVF_500	_TMA0
#define	TMA_OVF_250	_TMA1
#define	TMA_OVF_31_25	(_TMA1 | _TMA0)

#define	TCA	((volatile uint8_t *)0xffa7)

#define	TCA_CLR()	*TMA |= (_TMA2 | _TMA3)
