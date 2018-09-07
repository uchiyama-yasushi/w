
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

/*
	Delay calibrate
*/
#define	INT1A_CLOCK_GET		0x00
FUNC (bios_delay_calibration)
	ENTER_16
#if 0	// lidt delay.
	pushw	%ax
	movw	$100,	%ax
	call 	bios_delay
	popw	%ax
#endif
	sti	// enable clock interupt.
	pushw	%ax
	movb	$INT1A_CLOCK_GET, %ah
	int	$0x1a
	popw	%ax
	movw	%dx,	%bx
	call	bios_delay
	pushw	%bx
	movb	$INT1A_CLOCK_GET, %ah
	int	$0x1a
	popw	%bx
	subw	%bx,	%dx
	movw	%dx,	%ax
	cli
	LEAVE_16
	ret

FUNC (bios_delay)
	ENTER_16
	movw	%ax,	%cx
1:	movw	$-1,	%ax
2:	dec	%ax
	test	%ax,	%ax
	jnz	2b
	loop	1b
	LEAVE_16
	ret

	.code32
FUNC (bios_delay32)
	movl	%eax,	%ecx
1:	loop	1b
	ret
