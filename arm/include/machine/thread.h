
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
  uint32_t psr;		// 0x00
  uint32_t a1;		// 0x04 r0	caller saved / 1st arg
  uint32_t a2;		// 0x08 r1	caller saved / 2nd arg
  uint32_t a3;		// 0x0c r2	caller saved / 3rd arg
  uint32_t a4;		// 0x10 r3	caller saved / 4th arg
  uint32_t v1;		// 0x14 r4
  uint32_t v2;		// 0x18 r5
  uint32_t v3;		// 0x1c r6
  uint32_t v4;		// 0x20 r7
  uint32_t v5;		// 0x24 r8
  uint32_t v6;		// 0x28 r9
  uint32_t v7;		// 0x2c r10
  uint32_t fp;		// 0x30 r11
  uint32_t ip;		// 0x34 r12
  uint32_t sp;		// 0x38 r13
  uint32_t lr;		// 0x3c r14
  uint32_t pc;		// 0x40 r15
} __attribute__((packed)); //68Byte

#ifndef THREAD_STACKSIZE_DEFAULT
#define	THREAD_STACKSIZE_DEFAULT	1024
#endif

#endif
