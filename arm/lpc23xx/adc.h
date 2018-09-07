
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

// 10bit Analog-to-Digital Converter. 0-3V
#ifndef _LPC23XX_ADC_H_
#define	_LPC23XX_ADC_H_
// A/D Control Register
#define	ADC_CR		((volatile uint32_t *)0xe0034000)
#define	 CR_MASK	0x0f2fffff
// Select AD0.7:0 pins to be sampled.
#define	 CR_SEL0	0x00000001
#define	 CR_SEL1	0x00000002
#define	 CR_SEL2	0x00000004
#define	 CR_SEL3	0x00000008
#define	 CR_SEL4	0x00000010
#define	 CR_SEL5	0x00000020
#define	 CR_SEL6	0x00000040
#define	 CR_SEL7	0x00000080
// PCLK Divider. should be less than or equal 4.5MHz
#define	 CR_CLKDIV_MASK		0xff
#define	 CR_CLKDIV_SHIFT	8
// Burst conversion / software controlled.
#define	 CR_BURST	0x00010000
// # of clocks used for each conversion in Burst mode.
#define	 CR_CLKS_11	0x00000000
#define	 CR_CLKS_10	0x00020000
#define	 CR_CLKS_9	0x00040000
#define	 CR_CLKS_8	0x00060000
#define	 CR_CLKS_7	0x00080000
#define	 CR_CLKS_6	0x000a0000
#define	 CR_CLKS_5	0x000c0000
#define	 CR_CLKS_4	0x000e0000
// Power-down mode.
#define	 CR_PDN		0x00200000	// 1: operational 0: power-down mode.
// Software controlled conversion start.
#define	 CR_START_NOW	0x01000000
#define	 CR_START_EINT0	0x02000000
#define	 CR_START_CAP01	0x03000000
#define	 CR_START_MAT01	0x04000000
#define	 CR_START_MAT03	0x05000000
#define	 CR_START_MAT10	0x06000000
#define	 CR_START_MAT11	0x07000000
// Edge select for CR_START_EINT0-CR_START_MAT11
#define	 CR_EDGE	0x08000000

// A/D Global Data Register
// contains the result of the most recent A/D conversion.
#define	ADC_GDR		((volatile uint32_t *)0xe0034004)
#define	 GDR_V_MASK	0x3ff
#define	 GDR_V_SHIFT	6
#define	 GDR_CHN_MASK	0x7
#define	 GDR_CHN_SHIFT	24
#define	 GDR_OVERRUN	0x40000000
#define	 GDR_DONE	0x80000000

// A/D Status Register
#define	ADC_STAT	((volatile uint32_t *)0xe0034030)	//RO
#define	 STAT_DONE_MASK		0xff
#define	 STAT_DONE_SHIFT	0
#define	 STAT_OVERRUN_MASK	0xff
#define	 STAT_OVERRUN_SHIFT	8
#define	 STAT_ADINT		0x00010000

// A/D Interrupt Enable Register
#define	ADC_INTEN	((volatile uint32_t *)0xe003400c)
#define	 INTEN_0	0x00000001
#define	 INTEN_1	0x00000002
#define	 INTEN_2	0x00000004
#define	 INTEN_3	0x00000008
#define	 INTEN_4	0x00000010
#define	 INTEN_5	0x00000020
#define	 INTEN_6	0x00000040
#define	 INTEN_7	0x00000080
#define	 INTEN_GLOBAL	0x00000100	// Global DONE flag.

// Channel 0 Data Register.
#define	ADC_DR_BASE	0xe0034010
#define	ADC_DR0		((volatile uint32_t *)0xe0034010)
#define	ADC_DR1		((volatile uint32_t *)0xe0034014)
#define	ADC_DR2		((volatile uint32_t *)0xe0034018)
#define	ADC_DR3		((volatile uint32_t *)0xe003401c)
#define	ADC_DR4		((volatile uint32_t *)0xe0034020)
#define	ADC_DR5		((volatile uint32_t *)0xe0034024)
// LPC2377/2388 and LPC2388 only
#define	ADC_DR6		((volatile uint32_t *)0xe0034028)
#define	ADC_DR7		((volatile uint32_t *)0xe003402c)

#define	 DR_V_MASK	0x3ff
#define	 DR_V_SHIFT	6
#define	 DR_OVERRUN	0x40000000
#define	 DR_DONE	0x80000000

__BEGIN_DECLS
void adc_init (void);
void adc_fini (void);
void adc_channel_enable (int);
void adc_channel_disable (int);
bool adc_conversion (int, uint32_t *);
__END_DECLS

#endif
