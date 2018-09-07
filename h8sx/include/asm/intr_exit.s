
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
/* assembler parts. h8300-*-as don't have line separator. */
/* interrupt handler entry sequence. reschedule on exit. */
#ifndef THREAD_DISABLE
#ifdef EXT_FRAME_INFO
	adds	#2, sp		// unwind software flags of exception frame
#endif
	and.b	#7, r4l		// extract EXR.I[0-2] 0: Z=1, 1-7:Z=0
	bne	1f		// if (Z == 0(interrupt nested)) goto 1
	// Prepare exception frame.
	subs	#2, sp
	bset	#0, @sp		// Set exception frame bit

	mov.l	@_current_thread, er1
	mov.l	sp, @(0, er1)	// Store current frame to TLS

	mov.l	#0, er0	// no continuation.

	jsr	_thread_context_switch

	mov.l	@_current_thread, er1	// Load new thread control block.
	mov.l	@(0, er1), sp	// Set new exception/switch frame.
#ifdef THREAD_DEBUG
	mov	sp, er0
	jsr	___thread_switch_assert
#endif
	btst	#0, @sp		// Check frame type.
	adds	#2, sp
	beq	2f	// If switch frame, don't restore er0-er3
#endif // !THREAD_DISABLE
1:	ldm.l	@sp+, er0-er3
2:	rte/l	er4-er6
