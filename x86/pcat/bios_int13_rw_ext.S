
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
	Ext.13h
	bios_int13_ext (uint16_t packet_addr(%ax), uint16_t command_addr(%dx))
	__attribute ((regparm (2)))
*/
FUNC (bios_int13_ext)
	ENTER_16
	movw	%ax,	%si	// disk_address_packet addr.
	movw	%dx,	%bx
	movb	1(%bx),	%ah	// command;
	movb	0(%bx),	%dl	// drive
	movb	$0,	%al	// Disk verify off.
	int	$0x13
	jc	1f
	xorw	%ax,	%ax	// success.
1:	LEAVE_16
	ret

FUNC (bios_int13_ext_present)
	ENTER_16
	movb	$0x41,	%ah
	movw	$0x55aa,%bx
	movb	$0x80,	%dl
	int	$0x13
	jc	1f
	cmp	$0xaa55,%bx
	jnz	1f
	movb	%cl,	%al	// Major version | Interface supprot bitmap.
	jmp	2f
1:	movw	$0xffff,%ax
2:	LEAVE_16
	ret
