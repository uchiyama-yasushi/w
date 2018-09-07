
/*-
 * Copyright (c) 2010 UCHIYAMA Yasushi.  All rights reserved.
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

#ifndef _MACHINE_THREAD_H_
#define	_MACHINE_THREAD_H_

// Stack frame when exception is occured.
struct exception_stack_frame
{
  uint32_t pc;
  uint32_t sr;
} __attribute__((packed));

// Frame must be reserved in exception
struct exception_frame
{
  uint32_t r0;	// return value (caller saved)
  uint32_t r1;	// scratch (caller saved)
  uint32_t r2;	// scratch (caller saved)
  uint32_t r3;	// scratch (caller saved)
  uint32_t r4;	// args (caller saved)
  uint32_t r5;	// args (caller saved)
  uint32_t r6;	// args (caller saved)
  uint32_t r7;	// args (caller saved)
  uint32_t r8;	// callee saved
  uint32_t r9;	// callee saved
  uint32_t r10;	// callee saved
  uint32_t r11;	// callee saved
  uint32_t r12;	// callee saved
  uint32_t r13;	// callee saved
  uint32_t r14;	// frame pointer (callee saved)
  uint32_t pr;
  uint32_t gbr;
  // MAC regsiters. in Renesas convention, these are callee saved.
  uint32_t mach;// caller saved
  uint32_t macl;// caller saved
  struct exception_stack_frame stack_frame;
} __attribute__((packed));

// Frame must be reserved in explicit thread switch.
struct switch_frame
{
  uint32_t r8;	// callee saved
  uint32_t r9;	// callee saved
  uint32_t r10;	// callee saved
  uint32_t r11;	// callee saved
  uint32_t r12;	// callee saved
  uint32_t r13;	// callee saved
  uint32_t r14;	// frame pointer (callee saved)
  // Compatibility for exception frame.
  uint32_t _pr;
  uint32_t _gbr;
  uint32_t _mach;// caller saved
  uint32_t _macl;// caller saved
  struct exception_stack_frame stack_frame;
} __attribute__((packed));

struct reg
{
  uint32_t sw_flags;
#define	THREAD_FRAME_TYPE_SWITCH	0
#define	THREAD_FRAME_TYPE_EXCEPTION	1
#define	THREAD_STATE_START		0x0100
#define	THREAD_STATE_SWITCH		0x0200
#define	THREAD_STATE_INTR_MASK		0x00ff
#define	THREAD_STATE_INTR_XXX0		0x0001
#define	THREAD_STATE_INTR_XXX2		0x0002
  struct exception_frame ef;
} __attribute__((packed));

#endif
