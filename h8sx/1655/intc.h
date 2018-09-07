
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

// H8SX/1655 Interrupt Control registers.
#ifndef _H8SX_1655_INTC_H_
#define	_H8SX_1655_INTC_H_

// INTerrupt Control Register
#define	INTCR		((volatile uint8_t *)0xffff32)
#define	 _INTCR_INTM1		(1 << 5)
#define	 _INTCR_INTM0		(1 << 4)
#define	 INTCR_MODE_CCR		0
#define	 INTCR_MODE_EXR		_INTCR_INTM1
#define	 INTCR_MODE_MASK	0x3
#define	 INTCR_MODE_SHIFT	4

#define	 INTCR_NMIEG		(1 << 3)

// Interrupt Priprity Register A-R
// Priority 0(lowest)-7(highest)

#define	IPRA_ADDR	0xfffd40
#define	IPRB_ADDR	0xfffd42
#define	IPRC_ADDR	0xfffd44
//#define	IPRD_ADDR	 /* H8SX/1655M group only */
#define	IPRE_ADDR	0xfffd48
#define	IPRF_ADDR	0xfffd4a
#define	IPRG_ADDR	0xfffd4c
#define	IPRH_ADDR	0xfffd4e
#define	IPRI_ADDR	0xfffd50
#define	IPRJ_ADDR	0xfffd52
#define	IPRK_ADDR	0xfffd54
#define	IPRL_ADDR	0xfffd56
#define	IPRM_ADDR	0xfffd58
#define	IPRN_ADDR	0xfffd5a
#define	IPRO_ADDR	0xfffd5c
//
#define	IPRQ_ADDR	0xfffd60
#define	IPRR_ADDR	0xfffd62

#define	IPRA		((volatile uint16_t *)IPRA_ADDR)
#define	IPRB		((volatile uint16_t *)IPRB_ADDR)
#define	IPRC		((volatile uint16_t *)IPRC_ADDR)

#define	IPRE		((volatile uint16_t *)IPRE_ADDR)
#define	IPRF		((volatile uint16_t *)IPRF_ADDR)
#define	IPRG		((volatile uint16_t *)IPRG_ADDR)
#define	IPRH		((volatile uint16_t *)IPRH_ADDR)
#define	IPRI		((volatile uint16_t *)IPRI_ADDR)
#define	IPRJ		((volatile uint16_t *)IPRJ_ADDR)
#define	IPRK		((volatile uint16_t *)IPRK_ADDR)
#define	IPRL		((volatile uint16_t *)IPRL_ADDR)
#define	IPRM		((volatile uint16_t *)IPRM_ADDR)
#define	IPRN		((volatile uint16_t *)IPRN_ADDR)
#define	IPRO		((volatile uint16_t *)IPRO_ADDR)

#define	IPRQ		((volatile uint16_t *)IPRQ_ADDR)
#define	IPRR		((volatile uint16_t *)IPRR_ADDR)

#define	IPR_MASK	0x7	//0: lowest, 7:highest priority.


/* Interrupt Enable Register R/W */
#define	INTC_IER	((volatile uint16_t *)0xffff34)
#define	 IER_IRQ0E	(1 << 0)
#define	 IER_IRQ1E	(1 << 1)
#define	 IER_IRQ2E	(1 << 2)
#define	 IER_IRQ3E	(1 << 3)
#define	 IER_IRQ4E	(1 << 4)
#define	 IER_IRQ5E	(1 << 5)
#define	 IER_IRQ6E	(1 << 6)
#define	 IER_IRQ7E	(1 << 7)
#define	 IER_IRQ8E	(1 << 8)
#define	 IER_IRQ9E	(1 << 9)
#define	 IER_IRQ10E	(1 << 10)
#define	 IER_IRQ11E	(1 << 11)
#define	 IER_IRQ14E	(1 << 14)	//H8SX/1655M group only.

/* Interrupt Status Register R/(W) Zero-write only */
#define	INTC_ISR	((volatile uint16_t *)0xffff36)
#define	 ISR_IRQ0F	(1 << 0)
#define	 ISR_IRQ1F	(1 << 1)
#define	 ISR_IRQ2F	(1 << 2)
#define	 ISR_IRQ3F	(1 << 3)
#define	 ISR_IRQ4F	(1 << 4)
#define	 ISR_IRQ5F	(1 << 5)
#define	 ISR_IRQ6F	(1 << 6)
#define	 ISR_IRQ7F	(1 << 7)
#define	 ISR_IRQ8F	(1 << 8)
#define	 ISR_IRQ9F	(1 << 9)
#define	 ISR_IRQ10F	(1 << 10)
#define	 ISR_IRQ11F	(1 << 11)
#define	 ISR_IRQ14F	(1 << 14)	//H8SX/1655M group only.

/* Interrupt Sense Control Register H/L */
#define	INTC_ISCRH	((volatile uint16_t *)0xfffd68)
#define	INTC_ISCRL	((volatile uint16_t *)0xfffd6a)
// For IRQ14, falling edge only.
#define	 ISCR_LEVEL_L		0
#define	 ISCR_FALLING_EDGE	1
#define	 ISCR_RISING_EDGE	2
#define	 ISCR_BOTH_EDGE		3


__BEGIN_DECLS
#define	IPRADDR_MASK	0xffffff
#define	IPRSHIFT_MASK	0x1f
#define	IPRSHIFT_SHIFT	24

#define	IPR_ADDR(x)	((volatile uint16_t *)((x) & IPRADDR_MASK))
#define	IPR_SHIFT(x)	(((x) >> IPRSHIFT_SHIFT) & IPRSHIFT_MASK)

enum interrupt_priority
{
  INTPRI_0,	// Lowest
  INTPRI_1,
  INTPRI_2,
  INTPRI_3,
  INTPRI_4,
  INTPRI_5,
  INTPRI_6,
  INTPRI_7,	// Highest
};

enum interrupt_priority_module
{
#define	_(a, s)	IPR ## a ## _ADDR | (s << IPRSHIFT_SHIFT)
  // External pin
  INTPRI_IRQ0 = _(A, 12),
  INTPRI_IRQ1 = _(A, 8),
  INTPRI_IRQ2 = _(A, 4),
  INTPRI_IRQ3 = _(A, 0),
  INTPRI_IRQ4 = _(B, 12),
  INTPRI_IRQ5 = _(B, 8),
  INTPRI_IRQ6 = _(B, 4),
  INTPRI_IRQ7 = _(B, 0),
  INTPRI_IRQ8 = _(C, 12),
  INTPRI_IRQ9 = _(C, 8),
  INTPRI_IRQ10 = _(C, 4),
  INTPRI_IRQ11 = _(C, 0),
  // Low Voltage Detect (1655M group only.)
  //  INTPRI_IRQ14 = _(D, 4),
  // WDT
  INTPRI_WOVI = _(E, 8),
  // A/D Converter
  INTPRI_ADI0 = _(F, 8),
  // 16bit Timer Pulse Unit
  INTPRI_TPU0 = _(F, 4),
  INTPRI_TPU1 = _(F, 0),
  INTPRI_TPU2 = _(G, 12),
  INTPRI_TPU3 = _(G, 8),
  INTPRI_TPU4 = _(G, 4),
  INTPRI_TPU5 = _(G, 0),
  // 8bit Timer
  INTPRI_TMR0 = _(H, 12),
  INTPRI_TMR1 = _(H, 8),
  INTPRI_TMR2 = _(H, 4),
  INTPRI_TMR3 = _(H, 0),
  // DMA
  INTPRI_DMAC_DMTEND0 = _(I, 12),
  INTPRI_DMAC_DMTEND1 = _(I, 8),
  INTPRI_DMAC_DMTEND2 = _(I, 4),
  INTPRI_DMAC_DMTEND3 = _(I, 0),
  INTPRI_EXDMAC_EXDMTEND0 = _(J, 12),
  INTPRI_EXDMAC_EXDMTEND1 = _(J, 8),
  INTPRI_EXDMAC_EXDMTEND2 = _(J, 4),
  INTPRI_EXDMAC_EXDMTEND3 = _(J, 0),
  INTPRI_DMAC_DMEEND0_3 = _(K, 12),
  INTPRI_EXDMAC_EXDMEEND0_3 = _(K, 8),
  INTPRI_SCI0 = _(K, 4),
  INTPRI_SCI1 = _(K, 0),
  INTPRI_SCI2 = _(L, 12),
  INTPRI_SCI4 = _(L, 4),
  INTPRI_TPU6_TGI = _(L, 0),
  INTPRI_TPU6_TCI = _(M, 12),
  INTPRI_TPU7_TGI_AB = _(M, 8),
  INTPRI_TPU7_TGIV_TCIU = _(M, 4),
  INTPRI_TPU8_TGI_AB = _(M, 0),
  INTPRI_TPU8_TGIV_TCIU = _(N, 12),
  INTPRI_TPU9_TGI = _(N, 8),
  INTPRI_TPU9_TCI = _(N, 4),
  INTPRI_TPU10_TGI_AB = _(N, 0),
  INTPRI_TPU10_TGIV_TCIU = _(O, 12),
  INTPRI_TPU11_TGI_AB = _(O, 8),
  INTPRI_TPU11_TGIV_TCIU = _(O, 4),
  INTPRI_IIC2_0_1 = _(Q, 4),
  INTPRI_SCI5 = _(Q, 0),
  INTPRI_SCI6 = _(R, 12),
  INTPRI_TMR_4_5_6_7 = _(R, 8),
  INTPRI_USB = _(R, 4),
  INTPRI_ADC1_USBresume = _(R, 0),
#undef _
};

enum interrupt_priority intc_priority (enum interrupt_priority_module,
				       enum interrupt_priority);
void intc_init (void);
void intc_irq_sense (int, int);

__END_DECLS
#endif
