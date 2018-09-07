
/*-
 * Copyright (c) 2008, 2011 UCHIYAMA Yasushi.  All rights reserved.
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
#include <asm/machine.h>
/*
	fiber switch don't need to store return address. because
	stored stack pointer is never accessed from other fiber.

	setjmp/longjmp need to store return address. because its
	stack pointer is overrided by next subroutine call.
*/

	H8ARCH
	.section .text
	.align 2
	/* void fiber_switch (fiber_t from, fiber_t to, uint32_t ret) */
	.globl _fiber_switch
_fiber_switch:
	; save current context.
	; no need to store caller saved and ccr.
	mov.l	er7,	@(0x0c, er0) ; sp (contain return address)
	mov.l	er6,	@(0x08, er0) ; callee saved
	mov.l	er5,	@(0x04, er0) ; callee saved
	mov.l	er4,	@(0x00, er0) ; callee saved
	; load next context.
	mov.l	@(0x0c, er1), er7 ; sp
	mov.l	@(0x08, er1), er6 ; callee saved
	mov.l	@(0x04, er1), er5 ; callee saved
	mov.l	@(0x00, er1), er4 ; callee saved
	mov.l	er2,	er0	  ; subroutine return value / 1st arg.
	rts

	.align 2
	/* int setjmp (jmp_buf) */
	.global _setjmp
_setjmp:
	mov.l	er4,	@er0
	mov.l	er5,	@(4, er0)
	mov.l	er6,	@(8, er0)
	mov.l	er7,	@(12, er0)
	mov.l	@er7,	er1
	mov.l	er1,	@(16, er0)	; return address
	xor	er0,	er0	; return 0
	rts

	.align 2
	/* int longjmp (jmp_buf, int) */
	.global _longjmp
_longjmp:
	mov.l	@er0+,	er4
	mov.l	@er0+,	er5
	mov.l	@er0+,	er6
	mov.l	@er0+,	er7
	mov.l	@er0,	er2
	mov.l	er2,	@er7	; return address
	mov.l	er1,	er0
	rts