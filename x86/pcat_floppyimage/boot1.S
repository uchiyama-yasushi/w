
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

// Floppy disk boot sector.
#include <x86/asm.h>

	.section .text
	.code16
	ljmp	$0,	$1f	// make sure CS:IP = 0:0x7c00
1:	cli
	xorw	%ax,	%ax
	movw	%ax,	%ds
	movw	%ax,	%ss
	movb	%dl,	_drive	// Store drive #.
	movw	$_stack_start,	%sp

	call	bios_console_init
	movb	_drive,	%al
	call	bios_print_digit
	// Load Sector 2-36 to 0x7e00 (0x7e0:0)
	// 0x7e00-0x8000 : NetBSD disklabel
	// 0x8000-0xa000 : boot2 code
	movb	$0,	_head
	movb	$0,	_track
	movb	$2,	_sector
	movb	$17,	_n_sector
	movb	$2,	_command
	movw	$0x7e0,	_segment
	movw	$0,	_offset
	movw	$packet,%ax
	call	bios_int13_rw
	test	%ax,	%ax
	jnz	disk_error

	movb	$1,	_head
	movb	$1,	_sector
	movb	$18,	_n_sector
	movw	$0xa00,	_segment
	movw	$packet,%ax
	call	bios_int13_rw
	test	%ax,	%ax
	jnz	disk_error
	// Jump to boot2 with changing protect-mode.
	movw	$0x8000, %ax
	// Pass boot drive # to boot2
	movb	_drive,	%dl
1:	call	jmp32
	jmp	1b

disk_error:
	movw	%ax,	%bx
	movw	$drive_error_message, %ax
	call	bios_print_string
	movw	%bx,	%ax
	call	bios_print_digit
1:	jmp	1b
	// NOTREACHED
	.section .data
	.balign 4
packet:
_drive:		.byte	1
_head:		.byte	1
_track:		.byte	1
_sector:	.byte	1
_n_sector:	.byte	1
_command:	.byte	1
_segment:	.short	1
_offset:	.short	1
	.balign 4
drive_error_message:
	.string "error: "
