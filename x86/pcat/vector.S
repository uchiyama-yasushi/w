
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

	// 0x00-0x1f ... Processor reserved.
	// 0x20-0xff ... User defined.
	//
	//  interrupt -+ INTR pin
	//             + NMI pin
	//
	//  exception -+ fault : before op.
	//             + trap  : after op.
	//             + aboart: can't restart.
	//
	.code32
	/* 0x20-0x2f : IRQ 0-15 */
FUNC (int20_irq0)	// Timer
	ENTER_INTERRUPT
	movl	$0,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int21_irq1)	// Keyboard
	ENTER_INTERRUPT
	movl	$1,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int22_irq2)
	ENTER_INTERRUPT
	movl	$2,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int23_irq3)
	ENTER_INTERRUPT
	movl	$3,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int24_irq4)
	ENTER_INTERRUPT
	movl	$4,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int25_irq5)
	ENTER_INTERRUPT
	movl	$5,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int26_irq6)
	ENTER_INTERRUPT
	movl	$6,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int27_irq7)
	ENTER_INTERRUPT
	movl	$7,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int28_irq8)
	ENTER_INTERRUPT
	movl	$8,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int29_irq9)
	ENTER_INTERRUPT
	movl	$9,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int2a_irq10)
	ENTER_INTERRUPT
	movl	$10,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int2b_irq11)
	ENTER_INTERRUPT
	movl	$11,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int2c_irq12)
	ENTER_INTERRUPT
	movl	$12,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int2d_irq13)
	ENTER_INTERRUPT
	movl	$13,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int2e_irq14)
	ENTER_INTERRUPT
	movl	$14,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret

FUNC (int2f_irq15)
	ENTER_INTERRUPT
	movl	$15,	%eax
	call	interrupt_handler_dispatch
	LEAVE_INTERRUPT
	iret
