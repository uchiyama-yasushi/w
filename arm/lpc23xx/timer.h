
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

#ifndef _LPC23XX_TIMER_H_
#define	_LPC23XX_TIMER_H_
#define	T0_BASE		0xe0004000
#define	T1_BASE		0xe0008000
#define	T2_BASE		0xe0070000
#define	T3_BASE		0xe0074000

#define	T_IR		0x00
#define	T_TCR		0x04
#define	T_TC		0x08
#define	T_PR		0x0c
#define	T_PC		0x10
#define	T_MCR		0x14
#define	T_MR0		0x18
#define	T_MR1		0x1c
#define	T_MR2		0x20
#define	T_MR3		0x24
#define	T_CCR		0x28
#define	T_CR0		0x2c
#define	T_CR1		0x30
#define	T_EMR		0x3c
#define	T_CTCR		0x70
// Timer 0
#define	T0IR		((volatile uint32_t *)(T0_BASE + T_IR))
#define	T0TCR		((volatile uint32_t *)(T0_BASE + T_TCR))
#define	 TCR_ENABLE	0x01
#define	 TCR_RESET	0x02
#define	T0TC		((volatile uint32_t *)(T0_BASE + T_TC))
#define	T0PR		((volatile uint32_t *)(T0_BASE + T_PR))
#define	T0PC		((volatile uint32_t *)(T0_BASE + T_PC))
#define	T0MCR		((volatile uint32_t *)(T0_BASE + T_MCR))
#define	T0MR0		((volatile uint32_t *)(T0_BASE + T_MR0))
#define	T0MR1		((volatile uint32_t *)(T0_BASE + T_MR1))
#define	T0MR2		((volatile uint32_t *)(T0_BASE + T_MR2))
#define	T0MR3		((volatile uint32_t *)(T0_BASE + T_MR3))
#define	T0CCR		((volatile uint32_t *)(T0_BASE + T_CCR))
#define	T0CR0		((volatile uint32_t *)(T0_BASE + T_CR0))	//R
#define	T0CR1		((volatile uint32_t *)(T0_BASE + T_CR1))	//R
#define	T0EMR		((volatile uint32_t *)(T0_BASE + T_EMR))
#define	T0CTCR		((volatile uint32_t *)(T0_BASE + T_CTCR))

// Timer 1
#define	T1IR		((volatile uint32_t *)(T1_BASE + T_IR))
#define	T1TCR		((volatile uint32_t *)(T1_BASE + T_TCR))
#define	 TCR_ENABLE	0x01
#define	 TCR_RESET	0x02
#define	T1TC		((volatile uint32_t *)(T1_BASE + T_TC))
#define	T1PR		((volatile uint32_t *)(T1_BASE + T_PR))
#define	T1PC		((volatile uint32_t *)(T1_BASE + T_PC))
#define	T1MCR		((volatile uint32_t *)(T1_BASE + T_MCR))
#define	T1MR0		((volatile uint32_t *)(T1_BASE + T_MR0))
#define	T1MR1		((volatile uint32_t *)(T1_BASE + T_MR1))
#define	T1MR2		((volatile uint32_t *)(T1_BASE + T_MR2))
#define	T1MR3		((volatile uint32_t *)(T1_BASE + T_MR3))
#define	T1CCR		((volatile uint32_t *)(T1_BASE + T_CCR))
#define	T1CR0		((volatile uint32_t *)(T1_BASE + T_CR0))	//R
#define	T1CR1		((volatile uint32_t *)(T1_BASE + T_CR1))	//R
#define	T1EMR		((volatile uint32_t *)(T1_BASE + T_EMR))
#define	T1CTCR		((volatile uint32_t *)(T1_BASE + T_CTCR))

// Timer 2
#define	T2IR		((volatile uint32_t *)(T2_BASE + T_IR))
#define	T2TCR		((volatile uint32_t *)(T2_BASE + T_TCR))
#define	 TCR_ENABLE	0x01
#define	 TCR_RESET	0x02
#define	T2TC		((volatile uint32_t *)(T2_BASE + T_TC))
#define	T2PR		((volatile uint32_t *)(T2_BASE + T_PR))
#define	T2PC		((volatile uint32_t *)(T2_BASE + T_PC))
#define	T2MCR		((volatile uint32_t *)(T2_BASE + T_MCR))
#define	T2MR0		((volatile uint32_t *)(T2_BASE + T_MR0))
#define	T2MR1		((volatile uint32_t *)(T2_BASE + T_MR1))
#define	T2MR2		((volatile uint32_t *)(T2_BASE + T_MR2))
#define	T2MR3		((volatile uint32_t *)(T2_BASE + T_MR3))
#define	T2CCR		((volatile uint32_t *)(T2_BASE + T_CCR))
#define	T2CR0		((volatile uint32_t *)(T2_BASE + T_CR0))	//R
#define	T2CR1		((volatile uint32_t *)(T2_BASE + T_CR1))	//R
#define	T2EMR		((volatile uint32_t *)(T2_BASE + T_EMR))
#define	T2CTCR		((volatile uint32_t *)(T2_BASE + T_CTCR))

// Timer 3
#define	T3IR		((volatile uint32_t *)(T3_BASE + T_IR))
#define	T3TCR		((volatile uint32_t *)(T3_BASE + T_TCR))
#define	 TCR_ENABLE	0x01
#define	 TCR_RESET	0x02
#define	T3TC		((volatile uint32_t *)(T3_BASE + T_TC))
#define	T3PR		((volatile uint32_t *)(T3_BASE + T_PR))
#define	T3PC		((volatile uint32_t *)(T3_BASE + T_PC))
#define	T3MCR		((volatile uint32_t *)(T3_BASE + T_MCR))
#define	T3MR0		((volatile uint32_t *)(T3_BASE + T_MR0))
#define	T3MR1		((volatile uint32_t *)(T3_BASE + T_MR1))
#define	T3MR2		((volatile uint32_t *)(T3_BASE + T_MR2))
#define	T3MR3		((volatile uint32_t *)(T3_BASE + T_MR3))
#define	T3CCR		((volatile uint32_t *)(T3_BASE + T_CCR))
#define	T3CR0		((volatile uint32_t *)(T3_BASE + T_CR0))	//R
#define	T3CR1		((volatile uint32_t *)(T3_BASE + T_CR1))	//R
#define	T3EMR		((volatile uint32_t *)(T3_BASE + T_EMR))
#define	T3CTCR		((volatile uint32_t *)(T3_BASE + T_CTCR))

// Software definition
__BEGIN_DECLS
enum timer_channel
{
  TIMER0,
  TIMER1,
  TIMER2,
  TIMER3,
};

enum timer_config
{
  TIMER_INTR_ENABLE	= 0x0001,
  TIMER_COUNTER_RESET	= 0x0002,
  TIMER_COUNTER_STOP	= 0x0004,
};

enum timer_unit
{
  TIMER_SEC  = 1,
  TIMER_MSEC = 1000,
  TIMER_USEC = 1000000,
};

void __timer_config (enum timer_channel, int, enum timer_unit);
void timer_init (void);
void timer_fini (void);
void timer_start_nointr (enum timer_channel);
__END_DECLS
#endif
