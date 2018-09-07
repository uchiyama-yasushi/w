
/*-
 * Copyright (c) 2008 UCHIYAMA Yasushi.  All rights reserved.
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

#ifndef _INTC_H_
#define	_INTC_H_

/* IRQ Sense Control Register */
#define	INTC_ISCR		((volatile uint8_t *)0xfffff4)
#define	 ISCR_FALLING_EDGE	1
#define	 ISCR_LOW_LEVEL		0
/* IRQ Enable Register */
#define	INTC_IER		((volatile uint8_t *)0xfffff5)
#define	 IER_IRQ0E	0x01
#define	 IER_IRQ1E	0x02
#define	 IER_IRQ2E	0x04
#define	 IER_IRQ3E	0x08
#define	 IER_IRQ4E	0x10
#define	 IER_IRQ5E	0x20

/* IRQ Status Register */
#define	INTC_ISR		((volatile uint8_t *)0xfffff6)
#define	INTC_IPRA		((volatile uint8_t *)0xfffff8)
#define	INTC_IPRB		((volatile uint8_t *)0xfffff9)

#define	VECTOR_MIN		7	//NMI
#define	VECTOR_MAX		60

#endif



