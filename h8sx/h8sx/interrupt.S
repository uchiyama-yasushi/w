
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

// Interrupt control mode 2.
	.h8300sx
	.section .text

	.global _intr_status
_intr_status:
	/*
	uint8_t
	intr_status ()
	{
	  return *exr & 0x7;
	}
	*/
	stc.b	exr, r0l
	and.b	#0x7, r0l
	rts

	.global _intr_suspend
_intr_suspend:
	/*
	uint8_t
	intr_suspend ()
	{
	  r0 = *exr & 0x7;
	  *exr |= 0x7;
	  return r0;
	}
	*/
	stc.b	exr, r0l
	orc	#0x7, exr
	and.b	#0x7, r0l
	rts

	.global _intr_resume
_intr_resume:
	/*
	uint8_t
	intr_resume (uint8_t r0)
	{
	   r1 = *exr & 0xf8;
	   r0 |= r1;
	   *exr = r0;
	   return r0 & 0x7;
	}
	*/
	stc.b	exr, r1l
	and.b	#0xf8, r1l
	or.b	r1l, r0l
	ldc.b	r0l, exr
	and.b	#0x7, r0l
	rts

	.global _intr_priority
_intr_priority:
	/*
	uint8_t
	intr_priority (uint8_t r0)
	{
	  r2 = r0;
	  r1 = *exr;
	  r0 = r1 & 0x7;
	  r1 &= 0xf8;
	  r1 |= r2;
	  *exr = r1;
	  return r0;
	}
	*/
	mov.b	r0l, r2l
	stc.b	exr, r1l
	mov.b	r1l, r0l
	and.b	#0x7, r0l
	and.b	#0xf8,r1l
	or.b	r2l, r1l
	ldc.b	r1l, exr
	rts
