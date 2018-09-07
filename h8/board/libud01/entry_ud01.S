
/*-
 * Copyright (c) 2008-2009 UCHIYAMA Yasushi.  All rights reserved.
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
	.h8300h

	.section .text
	.globl	start
start:
	; ud01 IO port setting.
	mov.b	#0xff,	r3l
	sub.b	r2l,	r2l

	mov.b	r3l,	@0xc5:8	; P4 DDR output
	mov.b	r2l,	@0xda:8	; P4 PCR no pull-up
	mov.b	r2l,	@0xc7:8	; P4 DR = 0;

	mov.b	r2l,	@0xdb:8	; P5 PCR = 0

	mov.b	r2l,	@0xcb:8	; P6 DR = 0

	mov.b	r3l,	@0xcd:8	; P8 DDR output
	mov.b	r2l,	@0xcf:8	; P8 DR = 0;

	mov.b	r3l,	@0xd1:8	; PA output
	mov.b	r2l,	@0xd3:8	; PA DR = 0;

	mov.b	r3l,	@0xd4:8	; PB output
	mov.b	r2l,	@0xd6:8	; PB DR = 0;

	mov.b	#0xcf,	r3l	; ~0x30
	mov.b	r3l,	@0xd0:8	; P9 4,5 input

	; ud01 BSC setting
	mov.b	#0xff,	r3l
	mov.b	r3l,	@0xec:8

	mov.b	#0x70,	r0l
	mov.b	r0l,	@0xed:8

	mov.b	#0x3,	r2l
	mov.b	r2l,	@0xee:8

	mov.b	#0x40,	r2l
	mov.b	r2l,	@0xef:8

	sub.b	r2l,	r2l
	mov.b	r2l,	@0xf3:8

	mov.b	r0l,	@0x5f:8

	mov.b	r3l,	@0xc0:8
	mov.b	r3l,	@0xc1:8
	add.b	#0x1,	r2l
	mov.b	r2l,	@0xc8:8
	mov.b	#0x6,	r2l
	mov.b	r2l,	@0xc9:8
	; Now OK to use external RAM.

	; Set stack.
	mov.l	#_stack_start, sp
	; Set return address for NMI.
	mov.l	#start,	er0
	mov.l	er0,	@-sp
	mov.b	#0x80,	r0l
	mov.b	r0l,	@sp	; interrupt disable

loop:	jmp	@_machine_startup
	jmp	@loop
	/* NOTREACHED */
