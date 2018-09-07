
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

#include <x86/asm.h>
	/*
	void fiber_switch (fiber_t from, fiber_t to, uint32_t ret)
	__attribute ((regparm (3))) */
	.code32
FUNC (fiber_switch)
	//
	// save context. (from)
	movl	%esp, 0x10 (%eax)
	movl	%ebp, 0x0c (%eax)
	movl	%edi, 0x08 (%eax)
	movl	%esi, 0x04 (%eax)
	movl	%ebx, 0x00 (%eax)
	// load next context (to)
	movl	0x10 (%edx), %esp
	movl	0x0c (%edx), %ebp
	movl	0x08 (%edx), %edi
	movl	0x04 (%edx), %esi
	movl	0x00 (%edx), %ebx
	// subroutine return value/1st arg
	movl	%ecx,	%eax
	ret
