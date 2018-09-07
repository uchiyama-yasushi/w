
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

#ifndef _LPC23XX_VIC_H_
#define	_LPC23XX_VIC_H_
// Vectored Interrupt Controller

#define	VICIRQStatus		((volatile uint32_t *)0xfffff000)	//R
#define	VICFIQStatus		((volatile uint32_t *)0xfffff004)	//R

#define	VICRawIntr		((volatile uint32_t *)0xfffff008)	//R
#define	VICIntSelect		((volatile uint32_t *)0xfffff00c)	//RW
#define	VICIntEnable		((volatile uint32_t *)0xfffff010)	//RW
#define	VICIntEnClr		((volatile uint32_t *)0xfffff014)	// W

#define	VICSoftInt		((volatile uint32_t *)0xfffff018)	//RW
#define	VICSoftIntClear		((volatile uint32_t *)0xfffff01c)	// W

#define	VICProtection		((volatile uint32_t *)0xfffff020)	//RW
#define	VICSWPriorityMask	((volatile uint32_t *)0xfffff024)	//RW

#define	VICVectAddr		((volatile uint32_t *)0xfffff100)	//RW
#define	VICVectPriority		((volatile uint32_t *)0xfffff200)	//RW
// 15:lowest priority. 0:highest priority.

#define	VICAddress		((volatile uint32_t *)0xffffff00)	//RW

#define	VIC_WDT		0x00000001
#define	VIC_SOFTINT	0x00000002
#define	VIC_DBGCOMMRX	0x00000004
#define	VIC_DBGCOMMTX	0x00000008
#define	VIC_TIMER0	0x00000010
#define	VIC_TIMER1	0x00000020
#define	VIC_UART0	0x00000040
#define	VIC_UART1	0x00000080
#define	VIC_PWM1	0x00000100
#define	VIC_I2C0	0x00000200
#define	VIC_SPI		0x00000400
#define	VIC_SSP1	0x00000800
#define	VIC_PLL		0x00001000
#define	VIC_RTC		0x00002000
#define	VIC_EINT0	0x00004000
#define	VIC_EINT1	0x00008000
#define	VIC_EINT2	0x00010000
#define	VIC_EINT3	0x00020000
#define	VIC_ADC0	0x00040000
#define	VIC_I2C1	0x00080000
#define	VIC_BOD		0x00100000
#define	VIC_ETHER	0x00200000
#define	VIC_USB		0x00400000
#define	VIC_CAN		0x00800000
#define	VIC_SDMMC	0x01000000
#define	VIC_GPDMA	0x02000000
#define	VIC_TIMER2	0x04000000
#define	VIC_TIMER3	0x08000000
#define	VIC_UART2	0x10000000
#define	VIC_UART3	0x20000000
#define	VIC_I2C2	0x40000000
#define	VIC_I2S		0x80000000

__BEGIN_DECLS
enum vic_irq_type
{
  VIC_IRQ,
  VIC_FIQ,
};

void vic_init (void);
void vic_protect (bool);
int vic_priority_set (int, int);
int vic_priority_mask (int);
void vic_interrupt_enable (int, enum vic_irq_type);
void vic_interrupt_disable (int);

__END_DECLS
#endif
