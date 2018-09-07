
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
	/* void do_thread_switch (continuation_t) */
	/* Assume already interrupt disabled */
	/* Called from thread context */
	.code32
FUNC (do_thread_switch)
	/*
	Rearrange stack frame for IRET.
	     before   ---->  after
	|contnuation    ||               |
	+---------------++---------------+
ESP ->	|     EIP       ||    EFLAGS     |
	+---------------++---------------+
	|               ||    CS         |
	+---------------++---------------+
	|               ||    EIP        | <-ESP
	+---------------++---------------+
	|               || continuation  |
	+---------------++---------------+
	|               ||               |
        ...		 ...
	|               ||               |
	+---+---+---+---++---+---+---+---+ stack_top
	    3   2   1   0    3   2   1   0
	*/
	popl	%eax	// EIP
	pushf		// EFLAGS
	subl	%edx,	%edx
	movw	%cs,	%dx
	pushl	%edx	// CS
	pushl	%eax	// EIP
	//
	// save callee saved.	(caller saved is stored stack)
	movl	current_thread, %ecx
	movl	%esp, 0x1c (%ecx) //This %esp contains iret structure abobe.
	movl	%ebp, 0x18 (%ecx)
	movl	%edi, 0x14 (%ecx)
	movl	%esi, 0x10 (%ecx)
	movl	%ebx, 0x04 (%ecx)
	// no need to store caller saved. (this is subroutine call.)
	
	pushl	12(%esp)	// continuation.
	
	call	thread_context_switch
	addl	$4,	%esp
	movl	current_thread, %eax

	// restore all. thread may be suspended by interrupt.
	movl	0x1c (%eax), %esp
	movl	0x18 (%eax), %ebp
	movl	0x14 (%eax), %edi
	movl	0x10 (%eax), %esi
	movl	0x0c (%eax), %edx
	movl	0x08 (%eax), %ecx
	movl	0x04 (%eax), %ebx
	movl	0x00 (%eax), %eax
	//
	// resume EFLAGS/CS. and return
	iret
