
/*-
 * Copyright (c) 2008-2009 UCHIYAMA Yasushi.  All rights reserved.
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
  uint32_t er0;		// 0x00 caller saved	1st arg	/ return value
  uint32_t er1;		// 0x04 caller saved	2nd arg / return value
  uint32_t er2;		// 0x08 caller saved	3rd arg
  uint32_t er3;		// 0x0c caller saved
  uint32_t er4;		// 0x10
  uint32_t er5;		// 0x14
  uint32_t er6;		// 0x18
  uint32_t sp;		// 0x1c (er7)
  uint32_t ccr;		// 0x20 copy of the last ccr. == @er7 & 0xff000000
} __attribute__((packed)); //36Byte

#ifdef __NORMAL_MODE__
#define	ADDRESS_MASK	0x0000ffff	// 16bit address.
#else
#define	ADDRESS_MASK	0x00ffffff	// 24bit address.
#endif
#endif
