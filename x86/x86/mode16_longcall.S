
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
#include <gdt.h>
//----------------------------------------------------------------------
// int __lcall16 (int (*)(int, int), int, int)__attribute__((regparm (3)))
// call real-mode subroutine from protect-mode with segment switch.

FUNC (__lcall16)
	ENTER_32
	// suspend interrupt.
	pushf
	movl	(%esp),	%esi
	andl	$~0x200,(%esp)
	popf
	// Install Descriptor for real-mode.
	// Real-mode IDT
	lidt	realmode_lidt_arg

	movl	%esp,	%edi
	movw	$GDT_DATA32_BIOS, %bx
	movw	%bx,	%ss
	movw	%bx,	%ds
	movw	%bx,	%es
	movw	%bx,	%fs
	movw	%bx,	%gs
	// Change stack.
	movl	$0xfff0, %esp
	lcall	$GDT_CODE32_BIOS, $__lcall16_subr
	// Return to current segment.
	ljmp	$GDT_CODE32, $1f
1:	movw	$GDT_DATA32,	%bx
	movw	%bx,	%ss
	movw	%bx,	%ds
	movw	%bx,	%es
	movw	%bx,	%fs
	movw	%bx,	%gs
	movl	%edi,	%esp
#ifndef BOOTSTRAP_IDT
	// Restore IDT
	lidt	protectmode_lidt_arg
	// Resume interrupt
#endif
	pushf
	orl	%esi,	(%esp)
	popf
	LEAVE_32
	ret

	.section .data
	.balign 4
realmode_lidt_arg:
	.short	0x3ff
	.long	0x0
