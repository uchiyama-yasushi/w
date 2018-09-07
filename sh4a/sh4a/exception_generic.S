
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

#include <asm/asm.h>
	.section	.vector_generic, "ax"	// "A"llocate. e"X"ecute.
	// Use interrupted bank stack pointer.
	stc	spc,	r4	// Bank1
	stc	ssr,	r5	// Bank1
	stc	sgr,	r6	// Bank1
	mov.l	r8,	@-r15
	mov.l	r9,	@-r15
	mov.l	r10,	@-r15
	mov.l	r11,	@-r15
	mov.l	r12,	@-r15
	mov.l	r13,	@-r15
	mov.l	.L_exception_generic	r2
	jsr	@r2
	 mov.l	r14,	@-r15

	mov.l	@r15+,	r14
	mov.l	@r15+,	r13
	mov.l	@r15+,	r12
	mov.l	@r15+,	r11
	mov.l	@r15+,	r10
	mov.l	@r15+,	r9
	rte
	 mov.l	@r15+,	r8
	.align	2
.L_exception_generic:
	.long	_exception_generic