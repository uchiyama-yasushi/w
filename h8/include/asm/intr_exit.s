
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
/* assembler parts. h8300-*-as don't have line separator. */
/* interrupt handler entry sequence. reschedule on exit. */
	mov.l	#0,	er0	; no continuation.
	jsr	@_thread_context_switch
#ifdef __NORMAL_MODE__
	mov.w	@_current_thread, r0
#else
	mov.l	@_current_thread, er0
#endif
	mov.l	@(0x1c, er0), er7  ; sp
	mov.l	@(0x18, er0), er6 ; callee saved
	mov.l	@(0x14, er0), er5 ; callee saved
	mov.l	@(0x10, er0), er4 ; callee saved
	mov.l	@(0x0c, er0), er3 ; caller saved
	mov.l	@(0x08, er0), er2 ; caller saved
	mov.l	@(0x04, er0), er1 ; caller saved
	mov.l	@(0x00, er0), er0 ; caller saved
	; resume ccr and return.
	rte