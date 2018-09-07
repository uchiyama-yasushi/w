
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

	.h8300sx
	.section .text

	.align 2
	.global	_tmr0_cmia
_tmr0_cmia:
#include <asm/intr_enter.s>
	jsr	_c_tmr0_cmia
#include <asm/intr_exit.s>

	.global	_do_thread_switch_test
_do_thread_switch_test:
	mov.l	@sp, er1	// Return address.
	stc.b	exr, r2h	// Get current EXR

	mov.w	r2,  @-sp	// Store EXR
	stm.l	er4-er6, @-sp	// Store caller saved registers.
	subs	#2, sp		// Software flags region.
	bclr	#0, @sp		// clear exception frame bit

	mov.l	sp, @_current_thread	// Store current frame to TLS
	mov.l	sp, er2
	jsr	_thread_context_switch_test
	mov.l	@_current_thread, sp
	btst	#0, @sp
	adds	#2, sp
	beq	1f	// If exception frame, restore er0-er3
	// Restore registers.
	ldm.l	@sp+, er0-er3
1:	rte/l	er4-er6


	.align 2
	.global	_arg_test_caller
_arg_test_caller:
	stm.l	er4-er6, @-sp
	mov.b	#0x55, r3l
	mov.b	#0x34, r3h
	mov.b	#0x78, r4l
	mov	r3l, r0l
	mov	r3h, r1l
	mov	r4l, r2l
	jsr	_arg_test_callee
	rts/l	er4-er6
