
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
struct reg
{
  uint32_t pc;		// 0x
  uint32_t sr;		// 0x
  uint32_t pr;		// 0x
  // caller-saved(gcc) callee-saved(renesas)
  uint32_t mach;
  uint32_t macl;
  // callee-saved
  uint32_t sp;		// 0x
  uint32_t r14;		// 0x
  uint32_t r13;		// 0x
  uint32_t r12;		// 0x
  uint32_t r11;		// 0x
  uint32_t r10;		// 0x
  uint32_t r9;		// 0x
  uint32_t r8;		// 0x
  // caller-saved
  uint32_t r7;		// 0x
  uint32_t r6;		// 0x
  uint32_t r5;		// 0x
  uint32_t r4;		// 0x
  uint32_t r3;		// 0x
  uint32_t r2;		// 0x
  uint32_t r1;		// 0x
  uint32_t r0;		// 0x
} __attribute__((packed));

#ifndef THREAD_STACKSIZE_DEFAULT
#define	THREAD_STACKSIZE_DEFAULT	1024
#endif

#endif
