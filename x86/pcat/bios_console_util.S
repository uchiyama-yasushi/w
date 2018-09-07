
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
#define	PUTC(x)	mov $ ## x, %al	;call *bios_putc
	.comm	bios_putc	2
	.comm	bios_getc	2


FUNC (bios_console_init)
	ENTER_16
#ifdef SERIAL_CONSOLE
	movw	$bios_com_putc, bios_putc
	movw	$bios_com_getc, bios_getc
#else
	movw	$bios_vga_putc, bios_putc
	movw	$bios_kbd_getc, bios_getc
#endif
	LEAVE_16
	ret

/*
 void bios_console_init (int16_t console) __attribute__((regparm(1)))
	console :0 monitor
		 1 serial
*/

FUNC (bios_print_string)
	ENTER_16
	movw	%ax,	%si
	pushf
	cld			/* increment.*/
loop:
	lodsb
	cmp	$0,	%al
	jz	end
	call	*bios_putc
	jmp	loop
end:
	popf
	LEAVE_16
	ret

FUNC (bios_print_digit)
	ENTER_16
	movw	%ax,	%bx
	PUTC ('0')
	PUTC ('x')
	movw	$4,	%cx
loop3:	movw	%bx,	%ax
	shr	$0xc,	%ax
	cmp	$0x9,	%ax
	jnle	abcdef
	add	$'0',	%ax
	jmp	call_putc
abcdef:	add	$'W',	%ax
call_putc:
	call	*bios_putc
	shl	$4, 	%bx
	loop	loop3
	PUTC ('\r')
	PUTC ('\n')
	LEAVE_16
	ret
