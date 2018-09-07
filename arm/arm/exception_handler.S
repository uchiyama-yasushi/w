
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

	.balign 4
#ifndef THREAD_DISABLE
.Lcurrent_thread:
	.long	current_thread
#endif

FUNC (fiq_handler)
	stmdb	r13!,	{r0-r7, r14}
	bl	user_fiq_handler
	ldmia	r13!,	{r0-r7, r14}
	subs	pc,	r14,	#4

#ifdef THREAD_DISABLE
FUNC (irq_handler)
	stmdb	sp!,	{r0-r12, r14}
	bl	user_irq_handler
	ldmia	sp!,	{r0-r12, r14}
	subs	pc, lr, #4

FUNC (swi_handler)
	stmdb	sp!,	{r0-r12, r14}
	bl	user_swi_handler
	ldmia	sp!,	{r0-r12, r14}
	movs	pc, lr
#else
FUNC (irq_handler)
	adr	r13,	.Lcurrent_thread
	ldr	r13,	[r13]
	ldr	r13,	[r13]

	// save r0-r12 (skip PSR area)
	stmib	r13,	{r0-r12}

	// save psr
	mrs	r0,	spsr_all
	str	r0,	[r13]

	// set return address
	sub	r14,	r14,	#4	// IRQ,FIQ,Prefetch Abort.
	str	r14,	[r13, #0x40]
	mov	r1,	r13
	// change to system-mode
	mrs	r2,	cpsr_all
	orr	r2,	#0x1f
	msr	cpsr_c,	r2

	// save original r13,r14
	str	r13,	[r1, #0x38]
	str	r14,	[r1, #0x3c]
	// now interrupted thread's stack.

	bl	user_irq_handler

	// Switch thread if requested.
	mov	r0,	#0		// no continuation.
	bl	thread_context_switch
	adr	r0,	.Lcurrent_thread
	ldr	r0,	[r0]
	ldr	r0,	[r0]

	ldr	r1,	[r0], #4	// r1 = *r0++;
	msr	cpsr_all,r1		// restore all CPSR
	ldmia	r0,	{r0-r15}

FUNC (swi_handler)
	IRQ_DISABLE (r13)
	adr	r13,	.Lcurrent_thread
	ldr	r13,	[r13]
	ldr	r13,	[r13]

	// save r0-r12 (skip PSR area)
	stmib	r13,	{r0-r12}

	// save psr
	mrs	r0,	spsr_all
	str	r0,	[r13]

	// set return address
	str	r14,	[r13, #0x40]
	mov	r1,	r13
	// change to system-mode
	mrs	r2,	cpsr_all
	orr	r2,	#0x1f
	msr	cpsr_c,	r2

	// save original r13,r14
	str	r13,	[r1, #0x38]
	str	r14,	[r1, #0x3c]
	// now interrupted thread's stack.

	//------------------------------
	ldr	r0,	[r1, #0x40]	// return address.
	sub	r0,	#4		// swi instruction address
	ldr	r0,	[r0]		// swi instruction.
	and	r0,	#0xffffff	// user area.
	bl	user_swi_handler
	//------------------------------

	// Switch thread if requested.
	mov	r0,	#0		// no continuation.
	bl	thread_context_switch
	adr	r0,	.Lcurrent_thread
	ldr	r0,	[r0]
	ldr	r0,	[r0]

	ldr	r1,	[r0], #4	// r1 = *r0++;
	msr	cpsr_all,r1		// restore all CPSR
	ldmia	r0,	{r0-r15}
#endif	// THREAD_DISABLE

FUNC (exception_dabort)
	stmdb	sp!,	{r0-r12, r14}
	mov	r0,	lr
	mrs	r1,	spsr_all
	mov	r2,	sp
	bl	user_exception_dabort
	ldmia	sp!,	{r0-r12, r14}
	subs	pc,	lr, #8

FUNC (exception_iabort)
	stmdb	sp!,	{r0-r12, r14}
	mov	r0,	lr
	mrs	r1,	spsr_all
	mov	r2,	sp
	bl	user_exception_iabort
	ldmia	sp!,	{r0-r12, r14}
	subs	pc,	lr, #4

FUNC (exception_und)
	stmdb	sp!,	{r0-r12, r14}
	mov	r0,	lr
	mrs	r1,	spsr_all
	mov	r2,	sp
	bl	user_exception_und
	ldmia	sp!,	{r0-r12, r14}
	movs	pc,	lr
#if 0
FUNC (exception_test)
	stmdb	sp!,	{r0-r12, r14}
	ldmia	sp!,	{r0-r12, r14}
	mov	pc,	lr
#endif
