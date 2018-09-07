
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
// H8SX/1655 TMR module.

#ifndef _H8SX_1655_TMR_H_
#define	_H8SX_1655_TMR_H_

/*

Unit			0	0	1	1	2	2	3	3
Channel			0	1	2	3	4	5	6	7
---------------------------------------------------------------------------------
 internal clock		x	x	x	x	x	x	x	x
---------------------------------------------------------------------------------
 external clock		x	x	-	-	x	x	x	x
---------------------------------------------------------------------------------
 compare match intr.	x	x	x	x	*1	*1	*1     *1
---------------------------------------------------------------------------------
 overflow intr.		x	x	x	x	-	-	-	-
---------------------------------------------------------------------------------
 SCI5,6 clock out.	-	-	x	x	-	-	-	-
---------------------------------------------------------------------------------
 DTC kick		x	x	x	x	-	-	-	-
---------------------------------------------------------------------------------
 ADC kick		x	-	?	-	x	-	?	-

 *1) ORed
 */


#define	_TMR0_BASE	0xffffb0
#define	_TMR1_BASE	0xffffb1
#define	_TMR2_BASE	0xfffec0
#define	_TMR3_BASE	0xfffec1
#define	_TMR4_BASE	0xffea40
#define	_TMR5_BASE	0xffea41
#define	_TMR6_BASE	0xffea50
#define	_TMR7_BASE	0xffea51

/* Timer CouNTer */
#define	 _TMR_TCNT	0x8
/* Timer COnstant Register A */
#define	 _TMR_TCORA	0x4
/* Timer COnstant Register B */
#define	 _TMR_TCORB	0x6

/* Timer Control Register */
#define	 _TMR_TCR	0x0
// Compare Match Interrupt Enable A/B
#define	  TCR_CMIEB	(1 << 7)
#define	  TCR_CMIEA	(1 << 6)
// timer OVerflow Interrupt Enable (unit 0,1 only)
#define	  TCR_OVIE	(1 << 5)
// Counter CLeaR
#define	  TCR_CCLR1	(1 << 4)
#define	  TCR_CCLR0	(1 << 3)
#define	  TCR_CCLR_NO		0
#define	  TCR_CCLR_CMP_A	TCR_CCLR0
#define	  TCR_CCLR_CMP_B	TCR_CCLR1
#define	  TCR_CCLR_RESET	(TCR_CCLR0 | TCR_CCLR1)
// ClocK Select
#define	  TCR_CKS2	(1 << 2)
#define	  TCR_CKS1	(1 << 1)
#define	  TCR_CKS0	(1 << 0)

#define	  TCR_CKS_MASK	(TCR_CKS0 | TCR_CKS1 | TCR_CKS2)

/* Timer Counter Control Register */
#define	 _TMR_TCCR	0xa
// TiMer Reset Input Select (unit 0,1 only)
#define	  TCCR_TMRIS	(1 << 3)
// Internal ClocK Select
#define	  TCCR_ICKS1	(1 << 1)
#define	  TCCR_ICKS0	(1 << 0)
#define	  TCCR_ICKS_MASK	(TCCR_ICKS0 | TCCR_ICKS1)

// Clock sorce select
#define	TMR_CKS_NOCLK			0
	// External clock (unit 0,1,4,5,6,7)
#define	TMR_CKS_EXTCLK_RE		(TCR_CKS2|TCR_CKS0)	// Rising Edge
#define	TMR_CKS_EXTCLK_FE		(TCR_CKS2|TCR_CKS1)	// Falling Edge
#define	TMR_CKS_EXTCLK_BE		(TCR_CKS2|TCR_CKS1|TCR_CKS0)// Both Edge
	// Internal clock (unit 0,1,2,3,4,5,6,7)
#define	TMR_CKS_INTCLK_2_8		TCR_CKS0
#define	TMR_CKS_INTCLK_32_64		TCR_CKS1
#define	TMR_CKS_INTCLK_1024_8192	(TCR_CKS1|TCR_CKS0)
	// Counts at TCNT_1 overflow signal.
#define	TMR_CKS_OVF			TCR_CKS2

// Internal clock divider
#define	TMR_ICKS_div8_RE		0
#define	TMR_ICKS_div2_RE		TCCR_ICKS0
#define	TMR_ICKS_div8_FE		TCCR_ICKS1
#define	TMR_ICKS_div2_FE		(TCCR_ICKS1|TCCR_ICKS0)

#define	TMR_ICKS_div64_RE		0
#define	TMR_ICKS_div32_RE		TCCR_ICKS0
#define	TMR_ICKS_div64_FE		TCCR_ICKS1
#define	TMR_ICKS_div32_FE		(TCCR_ICKS1|TCCR_ICKS0)

#define	TMR_ICKS_div8192_RE		0
#define	TMR_ICKS_div1024_RE		TCCR_ICKS0
#define	TMR_ICKS_div8192_FE		TCCR_ICKS1
#define	TMR_ICKS_div1024_FE		(TCCR_ICKS1|TCCR_ICKS0)

/* Timer Control/Status Register */
#define	 _TMR_TCSR	0x2
// CoMpare match Flag B
#define	   TCSR_CMFB	(1 << 7)	//R/(W) zero-write only
// CoMpare match Flag A
#define	   TCSR_CMFA	(1 << 6)	//R/(W) zero-write only
// timer OVerflow Flag
#define	   TCSR_OVF	(1 << 5)	//R/(W) zero-write only
// A/D Trigger Enable (TMR0, 4 only)
#define	   TCSR_ADTE	(1 << 4)	//R/W
// Output Select
#define	   TCSR_OS3	(1 << 3)	//R/W
#define	   TCSR_OS2	(1 << 2)	//R/W
#define	   TCSR_OS1	(1 << 1)	//R/W
#define	   TCSR_OS0	(1 << 0)	//R/W


#define	TMR(n, r)	((volatile uint8_t *)_TMR ## n ## _BASE + _TMR_ ## r)

#define	TMR_GET_CNT(n)	(*TMR (n, TCNT))

__BEGIN_DECLS
struct tmr_config
{
  uint8_t TCORA;
  uint8_t TCORB;
  uint8_t TCR;
  uint8_t TCCR;
};

enum tmr_channel
  {
    TMR_CH0,
    TMR_CH1,
    TMR_CH2,
    TMR_CH3,
    TMR_CH4,
    TMR_CH5,
    TMR_CH6,
    TMR_CH7,
  };
#ifndef STANDALONE
void tmr_init (enum tmr_channel, enum interrupt_priority);
bool tmr_active (enum tmr_channel);
void tmr_fini (enum tmr_channel);

void tmr_start (enum tmr_channel, struct tmr_config *);
bool tmr_busy (enum tmr_channel);
uint8_t tmr_stop (enum tmr_channel);

uint8_t tmr_constant_calc (struct tmr_config *, uint32_t);
#endif
__END_DECLS

#endif
