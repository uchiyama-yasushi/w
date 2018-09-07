
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

// H8SX Power module.
#ifndef _H8SX_1655_POWER_H_
#define	_H8SX_1655_POWER_H_
// StanBY Control Register
#define	SBYCR		((volatile uint16_t *)0xfffdc6)
// Software stanby ; SLEEP inst. -> 0: sleep mode, 1: software stanby mode
#define	 SBYCR_SSBY		(1 << 15)	// (R/W|0)
// Output Port Enable ; SW stanby-mode -> 0: Hi-Z, 1: retaine
#define	 SBYCR_OPE		(1 << 14)
// Stanby Timer Select ; Select the time the MCU waits when wakeup.
// Default: 524288 state.
#define	 SBYCR_STS4		(1 << 12)	// (R/W|0)
#define	 SBYCR_STS3		(1 << 11)	// (R/W|1)
#define	 SBYCR_STS2		(1 << 10)	// (R/W|1)
#define	 SBYCR_STS1		(1 << 9)	// (R/W|1)
#define	 SBYCR_STS0		(1 << 8)	// (R/W|1)
// Sleep Instruction Exception Handling Enable
// 0: low-power state, 1: sleep exception.
#define	 SBYCR_SLPIE	(1 << 7)	// (R/W 0)

// Module SToP Control Register A
// 1: Stop, 0: Start

#define	MSTPCRA		((volatile uint16_t *)0xfffdc8)
// All module Clock Stop mode Enable
// 0: All-module-clock-stop mode disable, 1: enable.
#define	 MSTPCRA_ACSE		(1 << 15)	// (R/W|0)
#define	 MSTPCRA_EXDMAC		(1 << 14)	// (R/W|0)
#define	 MSTPCRA_DMAC		(1 << 13)	// (R/W|0)
#define	 MSTPCRA_DTC		(1 << 12)	// (R/W|0)
#define	 MSTPCRA_TMR_3_2	(1 << 9)	// (R/W|1)
#define	 MSTPCRA_TMR_1_0	(1 << 8)	// (R/W|1)
#define	 MSTPCRA_DAC_1_0	(1 << 5)	// (R/W|1)
#define	 MSTPCRA_ADC0		(1 << 3)	// (R/W|1)
#define	 MSTPCRA_TPU_11_6	(1 << 1)	// (R/W|1)
#define	 MSTPCRA_TPU_5_0	(1 << 0)	// (R/W|1)

// Module SToP Control Register B
#define	MSTPCRB		((volatile uint16_t *)0xfffdca)
#define	 MSTPCRB_PPG0		(1 << 15)	// (R/W|1)
#define	 MSTPCRB_SCI4		(1 << 12)	// (R/W|1)
#define	 MSTPCRB_SCI2		(1 << 10)	// (R/W|1)
#define	 MSTPCRB_SCI1		(1 << 9)	// (R/W|1)
#define	 MSTPCRB_SCI0		(1 << 8)	// (R/W|1)
#define	 MSTPCRB_IIC1		(1 << 7)	// (R/W|1)
#define	 MSTPCRB_IIC0		(1 << 6)	// (R/W|1)
#define	 MSTPCRB_UBC		(1 << 5)	// (R/W|1)

// Module SToP Control Register C
#define	MSTPCRC		((volatile uint16_t *)0xfffdcc)
#define	 MSTPCRC_SCI5		(1 << 15)	// (R/W|1)
#define	 MSTPCRC_SCI6		(1 << 14)	// (R/W|1)
#define	 MSTPCRC_TMR_4_5	(1 << 13)	// (R/W|1)
#define	 MSTPCRC_TMR_6_7	(1 << 12)	// (R/W|1)
#define	 MSTPCRC_USB		(1 << 11)	// (R/W|1)
#define	 MSTPCRC_CRC		(1 << 10)	// (R/W|1)
#define	 MSTPCRC_ADC1		(1 << 9)	// (R/W|1)
#define	 MSTPCRC_PPG1		(1 << 8)	// (R/W|1)
#define	 MSTPCRC_RAM_4A		(1 << 5)	// (R/W|0)
#define	 MSTPCRC_RAM_4B		(1 << 4)	// (R/W|0)
#define	 MSTPCRC_RAM_3_2A	(1 << 3)	// (R/W|0)
#define	 MSTPCRC_RAM_3_2B	(1 << 2)	// (R/W|0)
#define	 MSTPCRC_RAM_1_0A	(1 << 1)	// (R/W|0)
#define	 MSTPCRC_RAM_1_0B	(1 << 0)	// (R/W|0)

// DeeP StanBY Control Register
#define	DPSBYCR		((volatile uint8_t *)0xfffe70)
// notyet

// DeeP Stanby Wait Control Register
#define	DPSWCR		((volatile uint8_t *)0xfffe71)
// notyet

// DeeP Stanby Interrupt Enable Register
#define	DPSIER		((volatile uint8_t *)0xfffe72)
// notyet

// DeeP Stanby Interrupt Flag Register
#define	DPSIFR		((volatile uint8_t *)0xfffe73)
// notyet

// DeeP Stanby Interrupt EdGe Register
#define	DPSIEGR		((volatile uint8_t *)0xfffe74)
// notyet

// ReSeT Status Register
#define	RSTSR		((volatile uint8_t *)0xfffe75)
// notyet

// DeeP Stanby BacKup Register (0-15)
#define	DPSBKR0		((volatile uint8_t *)0xfffbf0)
#define	DPSBKR1		((volatile uint8_t *)0xfffbf1)
#define	DPSBKR2		((volatile uint8_t *)0xfffbf2)
#define	DPSBKR3		((volatile uint8_t *)0xfffbf3)
#define	DPSBKR4		((volatile uint8_t *)0xfffbf4)
#define	DPSBKR5		((volatile uint8_t *)0xfffbf5)
#define	DPSBKR6		((volatile uint8_t *)0xfffbf6)
#define	DPSBKR7		((volatile uint8_t *)0xfffbf7)
#define	DPSBKR8		((volatile uint8_t *)0xfffbf8)
#define	DPSBKR9		((volatile uint8_t *)0xfffbf9)
#define	DPSBKR10	((volatile uint8_t *)0xfffbfa)
#define	DPSBKR11	((volatile uint8_t *)0xfffbfb)
#define	DPSBKR12	((volatile uint8_t *)0xfffbfc)
#define	DPSBKR13	((volatile uint8_t *)0xfffbfd)
#define	DPSBKR14	((volatile uint8_t *)0xfffbfe)
#define	DPSBKR15	((volatile uint8_t *)0xfffbff)

__BEGIN_DECLS
enum module_power
{
  PWR_EXDMAC	= (0 << 16) | MSTPCRA_EXDMAC,
  PWR_DMAC	= (0 << 16) | MSTPCRA_DMAC,
  PWR_DTC	= (0 << 16) | MSTPCRA_DTC,
  PWR_TMR_2_3	= (0 << 16) | MSTPCRA_TMR_3_2,
  PWR_TMR_0_1	= (0 << 16) | MSTPCRA_TMR_1_0,
  PWR_DAC_0_1	= (0 << 16) | MSTPCRA_DAC_1_0,
  PWR_ADC0	= (0 << 16) | MSTPCRA_ADC0,
  PWR_TPU_6_11	= (0 << 16) | MSTPCRA_TPU_11_6,
  PWR_TPU_0_5	= (0 << 16) | MSTPCRA_TPU_5_0,
  PWR_PPG0	= (1 << 16) | MSTPCRB_PPG0,
  PWR_SCI4	= (1 << 16) | MSTPCRB_SCI4,
  PWR_SCI2	= (1 << 16) | MSTPCRB_SCI2,
  PWR_SCI1	= (1 << 16) | MSTPCRB_SCI1,
  PWR_SCI0	= (1 << 16) | MSTPCRB_SCI0,
  PWR_IIC1	= (1 << 16) | MSTPCRB_IIC1,
  PWR_IIC0	= (1 << 16) | MSTPCRB_IIC0,
  PWR_UBC	= (1 << 16) | MSTPCRB_UBC,
  PWR_SCI5	= (2 << 16) | MSTPCRC_SCI5,
  PWR_SCI6	= (2 << 16) | MSTPCRC_SCI6,
  PWR_TMR_4_5	= (2 << 16) | MSTPCRC_TMR_4_5,
  PWR_TMR_6_7	= (2 << 16) | MSTPCRC_TMR_6_7,
  PWR_USB	= (2 << 16) | MSTPCRC_USB,
  PWR_CRC	= (2 << 16) | MSTPCRC_CRC,
  PWR_ADC1	= (2 << 16) | MSTPCRC_ADC1,
  PWR_PPG1	= (2 << 16) | MSTPCRC_PPG1,
  PWR_RAM_4A	= (2 << 16) | MSTPCRC_RAM_4A,
  PWR_RAM_4B	= (2 << 16) | MSTPCRC_RAM_4B,
  PWR_RAM_2_3A	= (2 << 16) | MSTPCRC_RAM_3_2A,
  PWR_RAM_2_3B	= (2 << 16) | MSTPCRC_RAM_3_2B,
  PWR_RAM_0_1A	= (2 << 16) | MSTPCRC_RAM_1_0A,
  PWR_RAM_0_1B	= (2 << 16) | MSTPCRC_RAM_1_0B,
};

void peripheral_power (enum module_power, bool);
__END_DECLS


#endif
