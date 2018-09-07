
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

	.h8300h
	.section .text
	.text
	.globl _itu0_a
_itu0_a:
#include <asm/intr_enter.s>
	bclr #0, @0x60:8	;*ITU_TSTR &= ~ITU_TSTR_STR0;
	bclr #0, @0x66:8	;*ITU0_TIER &= ~ITU__TIER_IMIEA;
	bclr #0, @0x67:8	;*ITU0_TSR &= ~ITU__TSR_IMFA;
	jsr @_timer_do_schedule
#include <asm/intr_exit.s>


#ifndef THREAD_DISABLE
	.globl _itu1_a
_itu1_a:
#include <asm/intr_enter.s>
	bclr #1, @0x60:8	;*ITU_TSTR &= ~ITU_TSTR_STR0;
	bclr #0, @0x70:8	;*ITU1_TIER &= ~ITU__TIER_IMIEA;
	bclr #0, @0x71:8	;*ITU1_TSR &= ~ITU__TSR_IMFA;
	jsr @_timer_do_wakeup
#include <asm/intr_exit.s>
#endif // !THREAD_DISABLE