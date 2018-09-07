
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

#include <asm/machine.h>
	H8ARCH
	.section .text
	.globl	start
start:
	; Set stack.
	mov.w	#_stack_start, sp

	bset	#1,	@0xe0:8	; PMR1 |= PMR1_TXD

	mov.b	r0l,	 r0l
	mov.b	r0l,	@0xaa:8	; SCI_SCR3 = 0
	mov.b	r0l,	@0xa8:8	; SCI_SMR = 0 (8N1)
	mov.b	#8,	r0l
	mov.b	r0l,	@0xa9:8	; SCI_BRR = 8 (57600bps@16MHz)
	nop
	nop
	nop
	nop
	nop
	nop
	mov.b	#0x30,	r0l
	mov.b	r0l,	@0xaa:8	; SCI_SCR3 = SCR_TE | SCR_RE

busy:	btst	#7,	@0xac:8	;  Z = !(SCI_SSR & SSR_TDRE)
	beq	busy

	mov.b	#0x6f,	r0l	;'o'
	mov.b	r0l,	@0xab:8	; SCI_TDR = 'o'

	bra	busy

	; Set return address and CCR for NMI.
	mov.l	#start,	er0
	mov.l	er0,	@-sp
	mov.b	#0x80,	r0l
	mov.b	r0l,	@sp	; interrupt disable
loop:	/*jmp	@_machine_startup*/
	jmp	@loop
	/* NOTREACHED */
