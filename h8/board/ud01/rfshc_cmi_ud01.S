
/*-
 * Copyright (c) 2008 UCHIYAMA Yasushi.  All rights reserved.
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
	; Refresh Controller Compare Match Interrupt.
	.globl _rfshc_cmi
_rfshc_cmi:
#include <asm/intr_fast_enter.s>
	mov.l	er0, @-sp
	mov.b	@0xd2:8, r0l	; r0 = *P9_DR
	bclr	#7, @0xad:8	; *RFSHC_RTMCSR &= ~RTMCSR_CMF
	mov.l	er1, @-sp
	mov.w	@_la_sample_cnt, r1
	cmp.w	#0xff, r1
	bgt	rfshc_stop
	mov.b	r0l, @(_la_sample_data, r1)
	add.w	#1, r1
	mov.w	r1, @_la_sample_cnt
	mov.l	@sp+, er1
	mov.l	@sp+, er0
	rte
rfshc_stop:
	bclr	#6, @0xad:8	; *RFSHC_RTMCSR &= ~RTMCSR_CMIE
	mov.l	@sp+, er1
	mov.l	@sp+, er0
#include <asm/intr_fast_exit.s>
