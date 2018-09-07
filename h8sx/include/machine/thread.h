
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

struct exception_frame;

struct reg
{
  struct exception_frame *ef;
};

// Stack frame when exception is occured.
struct exception_stack_frame
{
  uint8_t exr;		/* 0x1e/0x0e EXR */
  uint8_t reserved;	/* 0x1f/0x0f reserved */
  uint32_t ccr_pc;	/* 0x20/0x10 CCR, 0x23 PC(24bit) */
} __attribute__((packed));

// Frame must be reserved in exception
struct exception_frame
{
  uint8_t flags;// frame type (switch(0)/exception(1))
  uint8_t thread_state;
#define	THREAD_STATE_START	0xff
#define	THREAD_STATE_SWITCH	0xfe
#define	THREAD_STATE_INTR_MASK	0xfe	// Free to use applications.
#define	THREAD_STATE_INTR_TMR7	1
#define	THREAD_STATE_INTR_TMR6	2

  uint32_t er3;	/* 0x02 caller saved */
  uint32_t er2;	/* 0x06 caller saved */
  uint32_t er1;	/* 0x0a caller saved */
  uint32_t er0;	/* 0x0e caller saved */
  uint32_t er6;	/* 0x12 callee saved */
  uint32_t er5;	/* 0x16 callee saved */
  uint32_t er4;	/* 0x1a callee saved */
  struct exception_stack_frame stack_frame;
} __attribute__((packed));

// Frame must be reserved in explicit thread switch.
struct switch_frame
{
  uint8_t flags;// frame type (switch/exception)
  uint8_t pad;	// for stack alignment
  uint32_t er6;	/* 0x02 callee saved */
  uint32_t er5;	/* 0x06 callee saved */
  uint32_t er4;	/* 0x0a callee saved */
  struct exception_stack_frame stack_frame;
} __attribute__((packed));
#endif
