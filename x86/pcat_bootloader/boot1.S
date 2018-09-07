
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

// MBR
#include <x86/asm.h>
	.section .text
	.code16
	.balign 2

	ljmp	$0, $start	// make sure CS:IP = 0:0x7c00
	// OEM name (3-10)
	.string	"<uch>"
	//
	// FAT12/16/32 BPB area.
	//
.org 0x3e
start:	cli
	xorw	%ax,	%ax
	movw	%ax,	%ds
	movw	%ax,	%ss
	movb	%dl,	drive	// Store drive #
	movw	$_stack_start,	%sp

	// Load Sector 2-62 to 0x8000 (0x800:0)
	// I assume sector/track is 63, partition is aligned by cylinder.
	movb	drive,	%dl
	movb	$0x02,	%ah	// Read
	movb	$61,	%al	// # of sectors to read.
	movb	$0,	%dh	// Head 0
	movb	$0,	%ch	// Cylinder 0
	movb	$2,	%cl	// Cylinder 0 Sector 2
	movw	$0x800,	%bx	// ES:BX = 0x800:0
	movw	%bx,	%es
	movw	$0,	%bx
	int	$0x13
	jc	disk_error
	// Jump to boot2
	movw	$0x8000, %ax
	// Pass boot drive # to boot2
	movb	drive,	%dl
	jmp	*%ax
disk_error:
	jmp	disk_error
	// NOTREACHED
	.lcomm	drive,	1


