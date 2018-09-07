
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
FUNC (task_exception_entry)
	mov	(%esp), %eax	// Error code.
	call	task_exception
	iret

FUNC (fault00_divided_by_zero)
	ENTER_EXCEPTION (0)
	call	exception_number_print
	LEAVE_EXCEPTION
	iret

FUNC (trap_fault01_debug)
	// fault: instruction address break point
	//	  generic
	// trap : data address break point
	//	: single step
	//	: task switch break point.
	ENTER_EXCEPTION (1)
	call	exception_number_print
	LEAVE_EXCEPTION
	iret

FUNC (trap02_nmi)
	ENTER_EXCEPTION (2)
	call	exception_number_print
	LEAVE_EXCEPTION
	iret

FUNC (trap03_break_point)
	ENTER_EXCEPTION (3)
	call	exception_number_print
	LEAVE_EXCEPTION
	iret

FUNC (trap04_overflow)
	ENTER_EXCEPTION (4)
	call	exception_number_print
	LEAVE_EXCEPTION
	iret

FUNC (fault05_bound_range_exceed)
	ENTER_EXCEPTION (5)
	call	exception_number_print
	LEAVE_EXCEPTION
	iret

FUNC (fault06_invalid_opcode)
	ENTER_EXCEPTION (6)
	call	exception_number_print
	LEAVE_EXCEPTION
	iret

FUNC (fault07_extension_not_available)
	ENTER_EXCEPTION (7)
	call	exception_number_print
	LEAVE_EXCEPTION
	iret

// Double fault (0x08) is task gate.

FUNC (abort09_cop_segment_overrun)
	ENTER_EXCEPTION (9)
	call	exception_number_print
	LEAVE_EXCEPTION
	iret

// Invalid TSS (0x0a) is task gate.

FUNC (fault0b_no_segment)
	ENTER_EXCEPTION_ARG (0xb)
	call	exception_number_print_with_arg
	LEAVE_EXCEPTION_ARG
	iret

// Stack overrun (0x0c) is task gate

FUNC (fault0d_general_protection)
	ENTER_EXCEPTION_ARG (0xd)
	call	exception_number_print_with_arg
	LEAVE_EXCEPTION_ARG
	iret

FUNC (fault0e_page_fault)
	ENTER_EXCEPTION_ARG (0xe)
	call	exception_number_print_with_arg
	LEAVE_EXCEPTION_ARG
	iret

	/* 0x0f intel reserved. */
FUNC (fault10_coprocessor_error)
	ENTER_EXCEPTION (0x10)
	call	exception_number_print
	LEAVE_EXCEPTION
	iret

FUNC (fault11_alignment)
	ENTER_EXCEPTION_ARG (0x11)
	call	exception_number_print_with_arg
	LEAVE_EXCEPTION_ARG
	iret

FUNC (abort12_machine_check)
	ENTER_EXCEPTION_ARG (0x12)
	call	exception_number_print_with_arg
	LEAVE_EXCEPTION_ARG
	iret
