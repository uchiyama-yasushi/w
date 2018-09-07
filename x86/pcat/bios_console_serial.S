
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
 Calling convention: __attribute__((regparm (3)))
 pass arguments AX, DX, and CX instead of on the stack.
*/
	.code16
FUNC (bios_com_init)
	ENTER_16
	movw	$0x00e3,%ax	// 9600 8N1
	int	$0x14
	LEAVE_16
	ret

FUNC (bios_com_putc)
	ENTER_16
	xor	%dx,	%dx	// COM1
	movb	$0x01,	%ah	// Send
	int	$0x14
	LEAVE_16
	ret

FUNC (bios_com_getc)
	ENTER_16
timeout:
	xor	%dx,	%dx	// COM1
	movb	$0x02,	%ah	// Receive
	int	$0x14
	// check timeout.
	test	$0xe000, %ax	// XXXQEMU Should be 0x8000
	jnz	timeout
	// loop until receive character.
	LEAVE_16
	ret

FUNC (bios_com_poll)
	ENTER_16
	xor	%dx,	%dx	// COM1
	movb	$0x02,	%ah	// Receive
	int	$0x14
	test	$0xe000, %ax	// XXXQEMU Should be 0x8000
	jz	1f
	xorw	%ax,	%ax
1:
	LEAVE_16
	ret
