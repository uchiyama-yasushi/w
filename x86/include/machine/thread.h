
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

#ifndef _MACHINE_THREAD_H_
#define	_MACHINE_THREAD_H_
// data structure for 'IRET' instruction. this data is constructed to
// bottom of stack.
struct iret_frame
{
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
  uint32_t noreturn;	// dummy return address.(never return)
  uint32_t arg;		// 1st arg.
} __attribute ((packed));

struct reg
{
  uint32_t eax;		// 0x00 caller saved
  uint32_t ebx;		// 0x04 callee saved
  uint32_t ecx;		// 0x08 caller saved
  uint32_t edx;		// 0x0c caller saved
  uint32_t esi;		// 0x10 callee saved
  uint32_t edi;		// 0x14 callee saved
  uint32_t ebp;		// 0x18 callee saved
  uint32_t sp;		// 0x1c callee saved (esp)
} __attribute__((packed)); //32Byte

#endif
