
/*-
 * Copyright (c) 2008 UCHIYAMA Yasushi.  All rights reserved.
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

#include <asm/machine.h>
	H8ARCH
	.section .text

	.global _intr_suspend
/*
	uint8_t
	intr_suspend ()
	{
	  r0 = *ccr & 0x80;
	  *ccr |= 0x80;
	  return r0;
	}
*/
_intr_suspend:
	stc.b	ccr, r0l
	orc	#0x80, ccr
	and.b	#0x80, r0l
	rts

	.global _intr_resume
/*
	uint8_t
	intr_resume (uint8_t r0)
	{
	   r1 = *ccr & 0x7f;
	   r0 |= r1;
	   *ccr = r0;
	   return r0 & 0x80;
	}
*/
_intr_resume:
	stc.b	ccr, r1l
	and.b	#0x7f, r1l
	or.b	r1l, r0l
	ldc.b	r0l, ccr
	and.b	#0x80, r0l
	rts
