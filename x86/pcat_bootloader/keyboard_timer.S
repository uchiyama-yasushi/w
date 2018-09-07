
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
	.code16
FUNC (kbd_poll_1sec)
	ENTER_16
	//drain buffer
	movb	$1,	%ah
	int	$0x16
	jz	1f
	movb	$0,	%ah
	int	$0x16		// drain buffer.
1:	movb	$2,	%ah	// Get RTC time.
	pushw	%bx
	int	$0x1a
	popw	%bx
	movb	%dh,	%bl	// sec
	cmp	%bh,	%bl
	movb	%bl,	%bh
	jz	1b
2:	movb	$1,	%ah
	pushw	%bx
	int	$0x16		// poll keyboard buffer
	popw	%bx
	jnz	4f		// jump if buffer is full.
	movb	$2,	%ah	// Get RTC time.
	pushw	%bx
	int	$0x1a
	popw	%bx
	movb	%dh,	%bl	// sec
	cmp	%bh,	%bl
	jz	2b
	xorw	%ax,	%ax
3:	LEAVE_16
	ret
4:	movb	$0,	%ah
	int	$0x16		// drain buffer.
	jmp	3b