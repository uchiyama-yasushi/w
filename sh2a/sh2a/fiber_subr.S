
/*-
 * Copyright (c) 2011 UCHIYAMA Yasushi.  All rights reserved.
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

/* void fiber_switch (fiber_t from, fiber_t to, uint32_t ret) */
FUNC (_fiber_switch)
/* save current context. */
	add	#36,	r4
	mov.l	r15,	@-r4
	mov.l	r14,	@-r4
	mov.l	r13,	@-r4
	mov.l	r12,	@-r4
	mov.l	r11,	@-r4
	mov.l	r10,	@-r4
	mov.l	r9,	@-r4
	mov.l	r8,	@-r4
	sts.l	pr,	@-r4
/* load next context. */
	lds.l	@r5+,	pr
	mov.l	@r5+,	r8
	mov.l	@r5+,	r9
	mov.l	@r5+,	r10
	mov.l	@r5+,	r11
	mov.l	@r5+,	r12
	mov.l	@r5+,	r13
	mov.l	@r5+,	r14
	mov.l	@r5+,	r15
	mov	r6,	r0	/* subroutine return value */
	rts
	 mov	r6,	r4	/* 1st arg. */

/* int setjmp (jmp_buf) */
FUNC (_setjmp)
/* save current context. */
	add	#36,	r4
	mov.l	r15,	@-r4
	mov.l	r14,	@-r4
	mov.l	r13,	@-r4
	mov.l	r12,	@-r4
	mov.l	r11,	@-r4
	mov.l	r10,	@-r4
	mov.l	r9,	@-r4
	mov.l	r8,	@-r4
	sts.l	pr,	@-r4
	rts
	 xor	r0, r0	/* return 0 */

/* void longjmp(jmp_buf, int) */
FUNC (_longjmp)
	lds.l	@r4+,	pr
	mov.l	@r4+,	r8
	mov.l	@r4+,	r9
	mov.l	@r4+,	r10
	mov.l	@r4+,	r11
	mov.l	@r4+,	r12
	mov.l	@r4+,	r13
	mov.l	@r4+,	r14
	mov.l	@r4+,	r15
	rts
	 mov	r5,	r0
