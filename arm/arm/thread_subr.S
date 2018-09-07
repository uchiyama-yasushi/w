
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
	/* void do_thread_switch (continuation_t) */
	/* Assume already interrupt disabled */
	/* Called from thread context */
	.long	current_thread
FUNC (do_thread_switch)
	// save context.
	mrs	r1,	cpsr
	ldr	r2,	[pc, #-16]	// r0 = current_thread
	ldr	r2,	[r2]		// r0 = *current_thread
	str	r1,	[r2], #20	// *r0 = r1, r0 += skip caller saved.
	stmia	r2!,	{r4-r15}	// save callee saved all.
	str	lr,	[r2, #-4]	// overwrite PC to LR. *(r0 -4) = lr
	bl	thread_context_switch
	ldr	r0,	[pc, #16]
	ldr	r0,	[r0]
	ldr	r1,	[r0], #4	// r1 = *r0++;
	msr	cpsr_cf,r1		// restore all CPSR
	ldmia	r0,	{r0-r15}
	bl	md_thread_noreturn_assert	// for debug.
	.long	current_thread

FUNC (__md_thread_jump)
	mov	sp,	r2
	mov	pc,	r1
	// NOTREACHED
