
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
 uint16_t bios_int13_rw (struct bios_int13_packet *)__attribute__ ((regparm(1)))
struct bios_int13_packet
{
  uint8_t drive;	//0 %dl
  uint8_t head;		//1 %dh
  uint8_t track;	//2 %ch
  uint8_t sector;	//3 %cl
  uint8_t n;		//4 %al
  uint8_t command;	//5 %ah
  uint16_t segment;	//6 %es
  uint16_t offset;	//8 %bx
} __attribute__((packed));

 RETURN VALUES: if success 0, else BIOS return value.
*/
	.code16
FUNC (bios_int13_rw)
	ENTER_16
	movw	%ax,	%bx	// struct bios_fdd
	movb	0(%bx),	%dl	// drive
	movb	1(%bx),	%dh	// head
	movb	2(%bx),	%ch	// track
	movb	3(%bx),	%cl	// sector
	movb	4(%bx),	%al	// # of sectors to read.
	movb	5(%bx),	%ah	// command (read 0x02, write 0x03)
	movw	6(%bx), %es	// buffer segment.
	movw	8(%bx), %bx	// buffer offset.
	int	$0x13
	jc	1f		// error check.
	xorw	%ax,	%ax	// return value. success -> 0
1:	LEAVE_16
	ret
