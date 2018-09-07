
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

	.text

	.align 8
	.global setjmp
	.type	setjmp, @function
setjmp:
	movq	%r12,	 (%rdi)
	movq	%r13,0x08(%rdi)
	movq	%r14,0x10(%rdi)
	movq	%r15,0x18(%rdi)
	movq	%rbx,0x20(%rdi)
	movq	%rbp,0x28(%rdi)
	movq	%rsp,0x30(%rdi)
	movq	(%rsp), %rax	// Get return address from stack.
	movq	%rax,0x38(%rdi)
	xorl	%eax,	%eax	// return 0
	ret
	.size	setjmp, .-setjmp

	.align 8
	.global longjmp
	.type	longjmp, @function
longjmp:
	movq	    (%rdi),	%r12
	movq	0x08(%rdi),	%r13
	movq	0x10(%rdi),	%r14
	movq	0x18(%rdi),	%r15
	movq	0x20(%rdi),	%rbx
	movq	0x28(%rdi),	%rbp
	movq	0x30(%rdi),	%rsp
	movq	0x38(%rdi),	%rax
	movq	%rax,	(%rsp)	// Set return address to stack.
	movl	%esi,	%eax	// return 2nd arg.
	ret
	.size	longjmp, .-longjmp
