
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

#include <arm/asm.h>

FUNC (cpu_psr_get)
	mrs	r0,	cpsr
	bx	lr

FUNC (intr_status)
	mrs	r0,	cpsr
	and	r0,	r0,	#0xc0
	bx	lr

FUNC (intr_disable)
	mrs	r0,	cpsr
	orr	r0,	r0,	#0xc0	// IRQ & FIQ
	msr	cpsr_c,	r0
	bx	lr

FUNC (intr_enable)
	mrs	r0,	cpsr
	bic	r0,	r0,	#0xc0
	msr	cpsr_c,	r0
	bx	lr

FUNC (intr_suspend)
	mrs	r1,	cpsr
	mov	r0,	r1
	orr	r1,	r1,	#0xc0
	msr	cpsr_c,	r1
	and	r0,	r0,	#0xc0
	bx	lr

FUNC (intr_resume)
	mrs	r1,	cpsr
	bic	r1,	r1,	#0xc0
	orr	r1,	r1,	r0
	msr	cpsr_c,	r1
	bx	lr

FUNC (cpu_regs_save)
	stmia	r0,	{r0-r15}
	bx	lr

FUNC (cpu_regs_load)
	ldmia	r0,	{r0-r15}
	bx	lr
