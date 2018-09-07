
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
	/* void port_polling (volatile uint8_t *port, int8_t bit_position, int32_t loop) */
	.global _port_polling
_port_polling:
	mov.l	er4,	@-sp
	mov.l	er5,	@-sp
	mov.l	er6,	@-sp

	mov.b	r1l,	r4h	; Target bit
	mov.l	#_la_high,	er5
	mov.l	#_la_low,	er6
	mov.l	er2,	er3
	shll	er3
	shll	er3
	add.l	er5,	er3	; er3 max loop

	sub.l	er2,	er2
	sub.l	er1,	er1

ph_loop: adds	#1,	er2	; high count
	mov.b	@er0,r4l	; r = *PX_DR
	btst	r4h,	r4l	; z = !(r & r4h)
	bne	ph_loop		;

	mov.l	er1, @er6	; 1st data of la_low[] is bogus.
	mov.l	er2, @er5
	adds	#4,  er5
	adds	#4,  er6
	sub.l	er2,	er2
	sub.l	er1,	er1

pl_loop: adds	#1,	er1	; low count
	mov.b	@er0,r4l	; r = *PX_DR
	btst	r4h,	r4l	; z = !(r & r4h)
	beq	pl_loop

	cmp.l	er5, er3
	bne	ph_loop

	mov.l	@sp+,	er6
	mov.l	@sp+,	er5
	mov.l	@sp+,	er4
	rts
