
/*-
 * Copyright (c) 2008-2009, 2011 UCHIYAMA Yasushi.  All rights reserved.
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

#ifndef _MACHINE_FIBER_H_
#define	_MACHINE_FIBER_H_
#define	FIBER_FUNC_ATTRIBUTE	//nothing.
struct fiber_reg
{
  uint32_t er4;		// 0x00
  uint32_t er5;		// 0x04
  uint32_t er6;		// 0x08
  uint32_t sp;		// 0x0c (er7)
} __attribute__((packed)); //16byte

struct jmp_reg
{
  uint32_t er4;		// 0x00
  uint32_t er5;		// 0x04
  uint32_t er6;		// 0x08
  uint32_t sp;		// 0x0c (er7)
  uint32_t return_addr;	// 0x10 (@er7)
} __attribute__((packed)); //20byte

// setjmp/longjmp interface
typedef uint32_t jmp_buf[sizeof (struct jmp_reg)];
#endif
