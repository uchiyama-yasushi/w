
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

#ifndef _X86_LOCAL_APIC_H_
#define	_X86_LOCAL_APIC_H_
#define	L_APIC_ADDR		0xfee00000

#define	L_APIC_		((volatile uint32_t *)0xfee0)

// Version Register (R)
#define	L_APIC_VER		((volatile uint32_t *)0xfee00030)
#define	 L_APIC_VER_MASK	0x000000ff
#define	 L_APIC_VER_MAXLVT	0x000f0000
#define	 L_APIC_VER_MAXLVT_SHIFT	16

/* Interrupt Destination */

// Physical destination (R/W)
#define	L_APIC_ID		((volatile uint32_t *)0xfee00020)
#define	 L_APIC_ID_MASK		0x0f000000
#define	 L_APIC_ID_SHIFT	24
// Logical Destination Register
#define	L_APIC_LDR		((volatile uint32_t *)0xfee000d0)
#define	L_APIC_LDR_MASK		0xff000000
// Destination Format Register
#define	L_APIC_DFR		((volatile uint32_t *)0xfee000e0)
#define	L_APIC_DFR_MASK		0xf0000000
#define	L_APIC_DFR_FLAT		0xf0000000
#define	L_APIC_DFR_CLUSTER	0x00000000

/* Local Vector Table */

#define	L_APIC_LVT_TIMER		((volatile uint32_t *)0xfee00320)
#define	L_APIC_LVT_LINT0		((volatile uint32_t *)0xfee00350)
#define	L_APIC_LVT_LINT1		((volatile uint32_t *)0xfee00360)
#define	L_APIC_LVT_ERROR		((volatile uint32_t *)0xfee00370)
#define	L_APIC_LVT_COUNTER		((volatile uint32_t *)0xfee00340) // -pentium pro

#define	L_APIC_LVT_VECTOR_MASK	0x000000ff
#define	L_APIC_LVT_MODE_FIXED	0x00000000
#define	L_APIC_LVT_MODE_NMI	0x00000400
#define	L_APIC_LVT_MODE_EXTINT	0x00000700
#define	L_APIC_LVT_STAT_PENDING	0x00001000
#define	L_APIC_LVT_STAT_IDLE	0x00000000
#define	L_APIC_LVT_POLAR	0x00002000
#define	L_APIC_LVT_REMOTE_IRR	0x00004000
#define	L_APIC_LVT_EDGE_TRIG	0x00000000
#define	L_APIC_LVT_LEVEL_TRIG	0x00008000
#define	L_APIC_LVT_INTR_MASK	0x00010000
#define	L_APIC_LVT_TIMER_PERIODIC	0x00020000
#define	L_APIC_LVT_TIMER_ONESHOT	0x00000000

// Interrupt Command Register
#define	L_APIC_ICR0		((volatile uint32_t *)0xfee00300)
#define	L_APIC_ICR1		((volatile uint32_t *)0xfee00310)

// Trigger Mode Register (R)(256bit) 0x180-0x1f0
#define	L_APIC_TMR		((volatile uint32_t *)0xfee00180)
// Interrupt Request Register (R)(256bit) 0x200-0x270
#define	L_APIC_IRR		((volatile uint32_t *)0xfee00200)
// Interrupt Service Register (R) (256bit) 0x100-0x170
#define	L_APIC_ISR		((volatile uint32_t *)0xfee00100)

/* Priority */

// Task Priority Register
#define	L_APIC_TPR		((volatile uint32_t *)0xfee00080)
#define	L_APIC_TPR_MASK		0x000000ff
// Processor Priority Register
#define	L_APIC_PPR		((volatile uint32_t *)0xfee000a0)
// Arbitration Priority Register
#define	L_APIC_APR		((volatile uint32_t *)0xfee00090)
/* End Of Interrupt (W) */
#define	L_APIC_EOI		((volatile uint32_t *)0xfee000b0)

// Spurious Vector Register
#define	L_APIC_SVR		((volatile uint32_t *)0xfee000f0)
#define	 SVR_VECTOR_MASK	0x000000f0
#define	 SVR_APIC_ENABLE	0x00000100
#define	 SVR_FOCUS_ENABLE	0x00000200

// Remote Read Register (R)
#define	L_APIC_RR		((volatile uint32_t *)0xfee000c0)

// Error Status Register (R)
#define	L_APIC_ESR		((volatile uint32_t *)0xfee00280)

/* Timer */

// Timer Divider Configuration Register (R/W)
#define	L_APIC_TDCR		((volatile uint32_t *)0xfee003e0)
// Timer Initial Count Register (R/W)
#define	L_APIC_TIC		((volatile uint32_t *)0xfee00380)
// Timer Current Count Register (R)
#define	L_APIC_TCC		((volatile uint32_t *)0xfee00390)

__BEGIN_DECLS
void l_apic_enable (int);
void l_apic_init (void);
__END_DECLS
#endif
