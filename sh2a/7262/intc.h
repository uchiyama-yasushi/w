
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

// SH2A 7262 Interrupt Control registers.
#ifndef _SH2A_7262_INTC_H_
#define	_SH2A_7262_INTC_H_

/* Interrupt Control Register */
// [NMI]
#define	INTC_ICR0	((volatile uint16_t *)0xfffe0800)
//  NMI input Level (0:low level)
#define	 ICR0_NMIL	(1 << 15)
//  NMI Edge select (0: falling edge, 1: rising edge)
#define	 ICR0_NMIE	(1 << 8)
//  (640KB model only) NMI interrupt status. (0: no request)
#define	 ICR0_NMIF	(1 << 1)
//  (640KB model only) NMI Mask (0:NMI interrupt enable)
#define	 ICR0_NMIM	(1 << 1)

// [IRQ sense select]
#define	INTC_ICR1	((volatile uint16_t *)0xfffe0802)
#define	 ICR1_LEVEL_L		0
#define	 ICR1_FALLING_EDGE	1
#define	 ICR1_RISING_EDGE	2
#define	 ICR1_BOTH_EDGE		3

// [PINT sense select]
#define	INTC_ICR2	((volatile uint16_t *)0xfffe0804)
#define	 ICR2_PINT0S	(1 << 0) // 0: Low level, 1: High level
#define	 ICR2_PINT1S	(1 << 1)
#define	 ICR2_PINT2S	(1 << 2)
#define	 ICR2_PINT3S	(1 << 3)
#define	 ICR2_PINT4S	(1 << 4)
#define	 ICR2_PINT5S	(1 << 5)
#define	 ICR2_PINT6S	(1 << 6)
#define	 ICR2_PINT7S	(1 << 7)

// IRQ Request Register (R/W) zero-write only.
#define	INTC_IRQRR	((volatile uint16_t *)0xfffe0806)
#define	 IRQRR_IRQ0F	(1 << 0)
#define	 IRQRR_IRQ1F	(1 << 1)
#define	 IRQRR_IRQ2F	(1 << 2)
#define	 IRQRR_IRQ3F	(1 << 3)
#define	 IRQRR_IRQ4F	(1 << 4)
#define	 IRQRR_IRQ5F	(1 << 5)
#define	 IRQRR_IRQ6F	(1 << 6)
#define	 IRQRR_IRQ7F	(1 << 7)

// PINT Enable Register (R/W)
#define	INTC_PINTER	((volatile uint16_t *)0xfffe0808)
#define	 PIRR_PINT0E	(1 << 0)
#define	 PIRR_PINT1E	(1 << 1)
#define	 PIRR_PINT2E	(1 << 2)
#define	 PIRR_PINT3E	(1 << 3)
#define	 PIRR_PINT4E	(1 << 4)
#define	 PIRR_PINT5E	(1 << 5)
#define	 PIRR_PINT6E	(1 << 6)
#define	 PIRR_PINT7E	(1 << 7)

// PINT Interrupt Request Register (Read only)
#define	INTC_PIRR	((volatile uint16_t *)0xfffe080a)
#define	 PIRR_PINT0R	(1 << 0)
#define	 PIRR_PINT1R	(1 << 1)
#define	 PIRR_PINT2R	(1 << 2)
#define	 PIRR_PINT3R	(1 << 3)
#define	 PIRR_PINT4R	(1 << 4)
#define	 PIRR_PINT5R	(1 << 5)
#define	 PIRR_PINT6R	(1 << 6)
#define	 PIRR_PINT7R	(1 << 7)

// Bank Control Register
#define	INTC_IBCR	((volatile uint16_t *)0xfffe080c)
#define	 IBCR_E1	(1 << 1)
#define	 IBCR_E2	(1 << 2)
#define	 IBCR_E3	(1 << 3)
#define	 IBCR_E4	(1 << 4)
#define	 IBCR_E5	(1 << 5)
#define	 IBCR_E6	(1 << 6)
#define	 IBCR_E7	(1 << 7)
#define	 IBCR_E8	(1 << 8)
#define	 IBCR_E9	(1 << 9)
#define	 IBCR_E10	(1 << 10)
#define	 IBCR_E11	(1 << 11)
#define	 IBCR_E12	(1 << 12)
#define	 IBCR_E13	(1 << 13)
#define	 IBCR_E14	(1 << 14)
#define	 IBCR_E15	(1 << 15)

// Bank Number Register
#define	INTC_IBNR	((volatile uint16_t *)0xfffe080e)
//  Bank Enable
#define	 IBNR_BE0	(1 << 14)
#define	 IBNR_BE1	(1 << 15)
#define	 IBNR_BE_DISABLE	0	 // Ignore IBCR
#define	 IBNR_BE_ENABLE		IBNR_BE0 // Ignore IBCR
#define	 IBNR_BE_IBCR		(IBNR_BE0|IBNR_BE1)
//  Bank OVer flow Enable
#define	 IBNR_OOVE	(1 << 13)	// Enable Exception
//  Bank Number (read only)
#define	 IBNR_BN	0xf

// Priority Register (0 lowest, 16 highest)
#define	INTC_IPR01_ADDR		0xfffe0818
#define	INTC_IPR02_ADDR		0xfffe081a
#define	INTC_IPR05_ADDR		0xfffe0820
#define	INTC_IPR06_ADDR		0xfffe0c00
#define	INTC_IPR07_ADDR		0xfffe0c02
#define	INTC_IPR08_ADDR		0xfffe0c04
#define	INTC_IPR09_ADDR		0xfffe0c06
#define	INTC_IPR10_ADDR		0xfffe0c08
#define	INTC_IPR11_ADDR		0xfffe0c0a
#define	INTC_IPR12_ADDR		0xfffe0c0c
#define	INTC_IPR13_ADDR		0xfffe0c0e
#define	INTC_IPR14_ADDR		0xfffe0c10
#define	INTC_IPR15_ADDR		0xfffe0c12
#define	INTC_IPR16_ADDR		0xfffe0c14
#define	INTC_IPR17_ADDR		0xfffe0c16
#define	INTC_IPR18_ADDR		0xfffe0c18
#define	INTC_IPR19_ADDR		0xfffe0c1a
#define	INTC_IPR20_ADDR		0xfffe0c1c
#define	INTC_IPR21_ADDR		0xfffe0c1e
#define	INTC_IPR22_ADDR		0xfffe0c20
#define	IPR_MASK		0xf

__BEGIN_DECLS
enum interrupt_priority
{
  INTPRI_0,	// Lowest
  INTPRI_1,
  INTPRI_2,
  INTPRI_3,
  INTPRI_4,
  INTPRI_5,
  INTPRI_6,
  INTPRI_7,
  INTPRI_8,
  INTPRI_9,
  INTPRI_10,
  INTPRI_11,
  INTPRI_12,
  INTPRI_13,
  INTPRI_14,
  INTPRI_15,
  INTPRI_16,	// Highest (NMI)
};

enum interrupt_priority_module
{
#define	_(a, s) (INTC_IPR ## a ## _ADDR << 4) | s
  INTPRI_IRQ0 = _(01, 12),
  INTPRI_IRQ1 = _(01, 8),
  INTPRI_IRQ2 = _(01, 4),
  INTPRI_IRQ3 = _(01, 0),

  INTPRI_IRQ4 = _(02, 12),
  INTPRI_IRQ5 = _(02, 8),
  INTPRI_IRQ6 = _(02, 4),
  INTPRI_IRQ7 = _(02, 0),

  INTPRI_PINT = _(05, 12),

  INTPRI_DMA0 = _(06, 12),
  INTPRI_DMA1 = _(06, 8),
  INTPRI_DMA2 = _(06, 4),
  INTPRI_DMA3 = _(06, 0),

  INTPRI_DMA4 = _(07, 12),
  INTPRI_DMA5 = _(07, 8),
  INTPRI_DMA6 = _(07, 4),
  INTPRI_DMA7 = _(07, 0),

  INTPRI_DMA8 = _(08, 12),
  INTPRI_DMA9 = _(08, 8),
  INTPRI_DMA10 = _(08, 4),
  INTPRI_DMA11 = _(08, 0),

  INTPRI_DMA12 = _(09, 12),
  INTPRI_DMA13 = _(09, 8),
  INTPRI_DMA14 = _(09, 4),
  INTPRI_DMA15 = _(09, 0),

  INTPRI_USB = _(10, 12),
  INTPRI_VIDEO = _(10, 8),
  INTPRI_CMT0 = _(10, 4),
  INTPRI_CMT1 = _(10, 0),

  INTPRI_BSC = _(11, 12),
  INTPRI_WDT = _(11, 8),
  INTPRI_TPU0A = _(11, 4),
  INTPRI_TPU0B = _(11, 0),

  INTPRI_TPU1A = _(12, 12),
  INTPRI_TPU1B = _(12, 8),
  INTPRI_TPU2A = _(12, 4),
  INTPRI_TPU2B = _(12, 0),

  INTPRI_TPU3A = _(13, 12),
  INTPRI_TPU3B = _(13, 8),
  INTPRI_TPU4A = _(13, 4),
  INTPRI_TPU4B = _(13, 0),

  INTPRI_PWM1 = _(14, 12),
  INTPRI_PWM2 = _(14, 8),
  INTPRI_ADC = _(14, 4),

  INTPRI_SOUND0 = _(15, 12),
  INTPRI_SOUND1 = _(15, 8),
  INTPRI_SOUND2 = _(15, 4),
  INTPRI_SOUND3 = _(15, 0),

  INTPRI_SPDIF = _(16, 12),
  INTPRI_I2C0 = _(16, 8),
  INTPRI_I2C1 = _(16, 4),
  INTPRI_I2C2 = _(16, 0),

  INTPRI_SCIF0 = _(17, 12),
  INTPRI_SCIF1 = _(17, 8),
  INTPRI_SCIF2 = _(17, 4),
  INTPRI_SCIF3 = _(17, 0),

  INTPRI_SCIF4 = _(18, 12),
  INTPRI_SCIF5 = _(18, 8),
  INTPRI_SCIF6 = _(18, 4),
  INTPRI_SCIF7 = _(18, 0),

  INTPRI_CLKSIO = _(19, 12),

  INTPRI_SPI0 = _(19, 4),
  INTPRI_SPI1 = _(19, 0),

  INTPRI_CAN0 = _(20, 12),
  INTPRI_CAN1 = _(20, 8),
  INTPRI_IEBUS = _(20, 4),
  INTPRI_CDROM = _(20, 0),

  INTPRI_NAND = _(21, 12),
  INTPRI_SDCARD = _(21, 8),
  INTPRI_RTC = _(21, 4),

  INTPRI_SAMPCONV0 = _(22, 12),
  INTPRI_SAMPCONV1 = _(22, 8),

  INTPRI_DECOMP = _(22, 0),
#undef _
};

enum interrupt_priority intc_priority (enum interrupt_priority_module,
				       enum interrupt_priority);
void intc_init (void);
void intc_irq_sense (int, int);
__END_DECLS

__END_DECLS
#endif
