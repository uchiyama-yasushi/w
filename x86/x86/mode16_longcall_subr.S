
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

	.code32
FUNC (__lcall16_subr)
	ENTER_32
	// GDT set segment limit for real-mode.
	movw	$GDT_DATA16,	%bx
	movw	%bx,	%ds
	movw	%bx,	%es
	movw	%bx,	%ss
	ljmp	$GDT_CODE16,	$1f	// Load CS descriptor for real-mode.
1:	.code16
	// Change to real-mode.
	movl	%cr0,	%ebx
	andl	$~1,	%ebx
	movl	%ebx,	%cr0
	ljmp	$REALMODE_CS,	$2f	// Load real-mode CS with pipeline flush.
2:
	// Set real-mode DS,ES,SS
	movw	$REALMODE_CS,	%bx
	mov	%bx,	%ds
	mov	%bx,	%es
	mov	%bx,	%ss
	// Now Real-mode.

	// Setup argument.
	mov	%ax,	%bx
	mov	%dx,	%ax
	mov	%cx,	%dx
	// Call requested function.
	call	*%bx

	// Return to Protect-mode.
	movl	%cr0,	%ebx
	orl	$0x1,	%ebx
	movl	%ebx,	%cr0
	ljmp	$GDT_CODE32_BIOS, $4f	// Load CS with pipeline flush.
4:	.code32
	// Load SS
	movw	$GDT_DATA32_BIOS,%bx
	movw	%bx,	%ss
	LEAVE_32
	lret
