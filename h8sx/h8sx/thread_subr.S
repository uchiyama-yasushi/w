
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
	/* void do_thread_switch (continuation_t) */
	/* Assume already interrupt disabled */
	/* Called from thread context */
	.global	_do_thread_switch
_do_thread_switch:
	// Construct dummy stack frame for rte or rte/l
	mov.w	#0, @-sp	// On resume, interrupt enabled.
				// No need to save EXR, CCR
				// Also zero clear ef->thread_state
	stm.l	er4-er6, @-sp	// Store callee saved registers.
	subs	#2, sp		// Software flags region.
	bclr	#0, @sp		// Mark 'switch frame'

	mov.l	@_current_thread, er4
	mov.l	sp, @(0, er4)	// Store current frame to TLS
	// Reschedule.
	jsr	_thread_context_switch
	// Run next thread.
	mov.l	@_current_thread, er4
	mov.l	@(0, er4), sp
	btst	#0, @sp	// If switch frame, Z = 1
	adds	#2, sp
	beq	1f	// If exception frame, restore er0-er3
	// Restore registers.
	ldm.l	@sp+, er0-er3
1:	rte/l	er4-er6
