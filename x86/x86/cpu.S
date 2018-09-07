
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
	.code32
FUNC (intr_suspend)
	pushf
	movl	(%esp),	%eax
	andl	$~0x200,(%esp)
	shr	$9,	%eax
	popf
	ret

FUNC (intr_resume)
	pushf
	shl	$9,	%eax
	orl	%eax,	(%esp)
	popf
	ret

FUNC (intr_status)
	pushf
	movl	(%esp),	%eax
	shr	$9,	%eax
	popf
	ret

FUNC (eflags_get)
	pushf
	movl	(%esp),	%eax
	popf
	ret

FUNC (eflags_set)
	pushf
	movl	%eax,	(%esp)
	popf
	ret

FUNC (cs_get)
	subl	%eax,	%eax
	movw	%cs,	%ax
	ret

FUNC (eip_get)
	movl	(%esp),	%eax
	ret

FUNC (iret_test)
	popl	%eax	// EIP
	pushf		// EFLAGS
	pushl	$0x08	// CS
	pushl	%eax	// EIP
	iret
/*
	rdmsr (%eax, %edx, %ecx) __attribute ((regparm (3)))
*/
FUNC (rdmsr)
	ENTER_32
	movl	%eax,	%esi
	movl	%edx,	%edi
	rdmsr
	movl	%eax,	(%esi)
	movl	%edx,	(%edi)
	LEAVE_32
	ret
/*
	wrmsr (%eax, %edx, %ecx) __attribute ((regparm (3)))
*/
FUNC (wrmsr)
	wrmsr
	ret
