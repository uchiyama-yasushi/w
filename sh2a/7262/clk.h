
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

// SH2A 7262 Clock Pulse Oscillator Unit.
#ifndef _SH2A_7262_CLK_H_
#define	_SH2A_7262_CLK_H_
  // Max Frequency
  // Ick 144MHz
  // Pck 36MHz
  // Bck 72MHz

/* FReQency Control Register */
#define	CLK_FRQCR	((volatile uint16_t *)0xfffe0010)
//  ClocK Output ENable 2 (CKIO pin)
#define	 FRQCR_CKOEN2	(1 << 14)	// 0: Unstable clock output, 1: low-level
//  ClocK Output ENable
#define	 FRQCR_CKOEN1	(1 << 13)
#define	 FRQCR_CKOEN0	(1 << 12)

// PLL multiply rate (R)
#define	 FRQCR_STC	(1 << 8)	//0: x8(mode 2,3), 1: x12 (mode 0,1)

// Internal Frequency
#define	 _FRQCR_IFC1	(1 << 5)
#define	 _FRQCR_IFC0	(1 << 4)
#define	FRQCR_IFC_SHIFT	4
#define	FRQCR_IFC_MASK	0x3

#define	FRQCR_IFC_x1	0
#define	FRQCR_IFC_x1_2	_FRQCR_IFC0
#define	FRQCR_IFC_x1_3	_FRQCR_IFC1

// Peripheral Frequency
#define	 _FRQCR_PFC2	(1 << 2)
#define	 _FRQCR_PFC1	(1 << 1)
#define	 _FRQCR_PFC0	(1 << 0)
#define	FRQCR_PFC_SHIFT	0
#define	FRQCR_PFC_MASK	0x7

#define	 FRQCR_PFC_x1_4		(_FRQCR_PFC0|_FRQCR_PFC1)
#define	 FRQCR_PFC_x1_6		_FRQCR_PFC2
#define	 FRQCR_PFC_x1_8		(_FRQCR_PFC2|_FRQCR_PFC0)
#define	 FRQCR_PFC_x1_12	(_FRQCR_PFC2|_FRQCR_PFC1)

__BEGIN_DECLS
enum clk_mode
{
  CLK_MODE_0,
  CLK_MODE_1,
  CLK_MODE_2,
  CLK_MODE_3,
};

void clk_init (enum clk_mode, int, uint16_t, uint16_t);
void clk_info ();

__END_DECLS
#endif
