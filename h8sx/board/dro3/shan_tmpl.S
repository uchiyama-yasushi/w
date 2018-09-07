
/*-
 * Copyright (c) 2008, 2010 UCHIYAMA Yasushi.  All rights reserved.
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
/* Data sampling routine for SHAN Digital Caliper */

#define	HI_LIMIT	#60
	.h8300sx
	.section .text
	.align 2

	.global FUNC_NAME
FUNC_NAME:	;;  LOW
#include <asm/intr_enter.s>
wait_rising_edge0:
	mov.b	CLK_PORT, r0l
	btst	CLK_BIT,  r0l
	beq	wait_rising_edge0
count_high_level:
	;;  HIGH
	mov.b	HI_LIMIT r0h	; limit count.
wait_falling_edge0:
	add.b	#-1,	r0h
	beq	error_return_0		; too long high region.
	mov.b	CLK_PORT, r0l
	btst	CLK_BIT,  r0l
	bne	wait_falling_edge0
	;;  LOW
	mov.b	#60	r0h
//	mov.b	#20	r0h
wait_rising_edge1:
	add.b	#-1,	r0h
	beq	sampling_start	; data phase start.
	mov.b	CLK_PORT, r0l
	btst	CLK_BIT,	r0l
	beq	wait_rising_edge1
	bra	count_high_level

sampling_start:
	;; still LOW.
	sub.l	er2,	er2	; caliper data
	mov.l	#1,	er3	; bit
	mov.b	#23,	r1l	; r1l counter. (clock)
wait_rising_edge2:
	mov.b	CLK_PORT, r0l	; waiting for rising edge.
	btst	CLK_BIT,  r0l
	beq	wait_rising_edge2
	;;
	;; data phase
	;;
	;; HIGH
sampling_loop:			; wait for falling edge.
	mov.b	HI_LIMIT r0h	; limit count.

	mov.b	DATA_PORT, r0l	; sample data bit.
	btst	DATA_BIT,  r0l
	beq	wait_falling_edge1
	or.l	er3, er2	; set data.
wait_falling_edge1:
	add.b	#-1,	r0h
	beq	error_return_1		; too long high region. return.
	mov.b	CLK_PORT, r0l
	btst	CLK_BIT,  r0l
	bne	wait_falling_edge1
	;; LOW
wait_rising_edge3:	; wait for rising edge
	mov.b	CLK_PORT, r0l
	btst	CLK_BIT,  r0l
	beq	wait_rising_edge3
	;; HIGH
	shll.l	er3	; next bit.
	;; 24clock?
	add.b	#-1,	r1l
	bpl	sampling_loop
;;; sampling done.
	mov.l	#_shan_data, er0
	mov.l	er2,	@(DATA_OFFSET, er0) 	; copy data.
	mov.l	#_shan_data_updated, er0
	bset	UPDATE_BIT,	@er0		; update flag.
	bra	success_return
error_return_0:
	mov.l	#_shan_error, er0
	bset	ERR0_BIT,	@er0
	bra	success_return
error_return_1:
	mov.l	#_shan_error, er0
	mov.b	r1l, @_shan_error_cause,
	bset	ERR1_BIT,	@er0
success_return:
	mov.l	@_caliper_manager_thread, er0
	jsr	@_thread_wakeup_once
	bclr	ISR_BIT,	ISR_REG	; ISR clear flag during data sampling.
#include <asm/intr_exit.s>

