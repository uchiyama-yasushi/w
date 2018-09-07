
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

// Pulse Width Modulator
#ifndef _LPC23XX_PWM_H_
#define	_LPC23XX_PWM_H_
// Interrupt Register
#define	PWM_IR		((volatile uint32_t *)0xe0018000)
#define	 IR_MR0		0x0001	// Interrupt flag for match channel 0
#define	 IR_MR1		0x0002
#define	 IR_MR2		0x0004
#define	 IR_MR3		0x0008
#define	 IR_CAP0	0x0010	// Interrupt flag for capture input 0
#define	 IR_CAP1	0x0020
#define	 IR_MR4		0x0100
#define	 IR_MR5		0x0200
#define	 IR_MR6		0x0400
#define	 IR_MASK	0x073f

// Timer Control Register
#define	PWM_TCR		((volatile uint32_t *)0xe0018004)
#define	 TCR_CENABLE	0x01	// Counter Enable
#define	 TCR_CRESET	0x02	// Counter Reset
#define	 TCR_PWMENABLE	0x08	// PWM Enable
#define	 TCR_MASK	0x0b

// Timer Counter
#define	PWM_TC		((volatile uint32_t *)0xe0018008)
// Prescale Register
#define	PWM_PR		((volatile uint32_t *)0xe001800c)
// Prescale Counter
#define	PWM_PC		((volatile uint32_t *)0xe0018010)
// Match Control Register
#define	PWM_MCR		((volatile uint32_t *)0xe0018014)
#define	 MCR_INTR	0x01
#define	 MCR_RESET	0x02
#define	 MCR_STOP	0x04
#define	 MCR_MR0_SHIFT	0
#define	 MCR_MR1_SHIFT	3
#define	 MCR_MR2_SHIFT	6
#define	 MCR_MR3_SHIFT	9
#define	 MCR_MR4_SHIFT	12
#define	 MCR_MR5_SHIFT	15
#define	 MCR_MR6_SHIFT	18
#define	 MCR_MASK	0x1fffff

// Match Register
#define	PWM_MR0		((volatile uint32_t *)0xe0018018)
#define	PWM_MR1		((volatile uint32_t *)0xe001801c)
#define	PWM_MR2		((volatile uint32_t *)0xe0018020)
#define	PWM_MR3		((volatile uint32_t *)0xe0018024)
// Capture Control Register
#define	PWM_CCR		((volatile uint32_t *)0xe0018028)
#define	 CCR_RE		0x01
#define	 CCR_FE		0x02
#define	 CCR_EVENT	0x04
#define	 CCR_CR0_SHIFT	0
#define	 CCR_CR1_SHIFT	3
#define	 CCR_CR2_SHIFT	6
#define	 CCR_CR3_SHIFT	9
#define	 CCR_MASK	0xfff

// Capture Register
#define	PWM_CR0		((volatile uint32_t *)0xe001802c)	// RO
#define	PWM_CR1		((volatile uint32_t *)0xe0018030)	// RO
#define	PWM_CR2		((volatile uint32_t *)0xe0018034)	// RO
#define	PWM_CR3		((volatile uint32_t *)0xe0018038)	// RO
// Match Register
#define	PWM_MR4		((volatile uint32_t *)0xe0018040)
#define	PWM_MR5		((volatile uint32_t *)0xe0018044)
#define	PWM_MR6		((volatile uint32_t *)0xe0018048)
// PWM Control Register
#define	PWM_PCR		((volatile uint32_t *)0xe001804c)
#define	 PCR_PWMSEL2	0x0004	// 1: double edge 0: single edge
#define	 PCR_PWMSEL3	0x0008
#define	 PCR_PWMSEL4	0x0010
#define	 PCR_PWMSEL5	0x0020
#define	 PCR_PWMSEL6	0x0040
#define	 PCR_PWMENA1	0x0200	// Output enable
#define	 PCR_PWMENA2	0x0400
#define	 PCR_PWMENA3	0x0800
#define	 PCR_PWMENA4	0x1000
#define	 PCR_PWMENA5	0x2000
#define	 PCR_PWMENA6	0x4000

// Latch Enable Register
#define	PWM_LER		((volatile uint32_t *)0xe0018050)
#define	 LER_MR0	0x01
#define	 LER_MR1	0x02
#define	 LER_MR2	0x04
#define	 LER_MR3	0x08
#define	 LER_MR4	0x10
#define	 LER_MR5	0x20
#define	 LER_MR6	0x40
#define	 LER_MASK	0x7f

// Count Control Register
#define	PWM_CTCR	((volatile uint32_t *)0xe0018070)
// Counter/Timer mode.
#define	 CTCR_TIMER		0x00	// Timer mode.
#define	 CTCR_COUNTER_R		0x01	// Rising edge
#define	 CTCR_COUNTER_F		0x02	// Falling edge
#define	 CTCR_COUNTER_RF	0x03	// Both edge
// Count Input Select
#define	 CTCR_PCAP10		0x00
#define	 CTCR_CAP11		0x04
#define	 CTCR_MASK		0x0f
#endif
