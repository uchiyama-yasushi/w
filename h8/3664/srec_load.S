
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

/* Motorola S-format loader for H8/3664,3694 */
	.h8300hn
	.section .text
	.globl _srec_load
_srec_load:
	mov.l	er4,	@-sp
	mov.l	er0	@-sp
	sub.l	er4,	er4
loop:	btst	#6,	@0xac:8	; *SCI_SSR & SSR_RDRF
	beq	loop
	mov.b	@0xad:8,r0l	; r0l = *SCI_RDR
	btst	#5,	@0xac:8	; *SCI_SSR & SSR_ORER
	bne	read_error	; error occured.
	add.s	#1,	er4

	jsr	@_srec_parser
	cmp.b	#0,	r0l
	beq	loop
	bra	end
read_error:
	mov.b	#5,	r0l
end:
	mov.l	@sp+,	er1
	mov.l	er4,	@er1
	mov.l	@sp+,	er4
	rts

