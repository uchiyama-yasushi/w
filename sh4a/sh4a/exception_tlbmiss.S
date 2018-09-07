
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

#include <asm/asm.h>
	.section	.vector_tlbmiss, "ax"	// "A"llocate. e"X"ecute.
	.align	2
	mov.l	r0,	@-r15
	mov.l	r1,	@-r15
	mov.l	r2,	@-r15
	mov.l	r3,	@-r15
	mov.l	r4,	@-r15
	mov.l	r5,	@-r15
	mov.l	r6,	@-r15
	mov.l	r7,	@-r15
	mov.l	r8,	@-r15
	mov.l	r9,	@-r15
	mov.l	r10,	@-r15
	mov.l	r11,	@-r15
	mov.l	r12,	@-r15
	mov.l	r13,	@-r15
	mov.l	r14,	@-r15
	sts.l	macl,	@-r15
	sts.l	mach,	@-r15
	sts.l	pr,	@-r15

	// All register saved.
/*
	// Set IMASK and Clear BL (Disable all interrupt.)
	stc	sr,	r0
	mov.l	.L_SR_IMASK,	r1
	or	r1,	r0
	mov.l	.L_SR_BL,	r1	// r1 = 0x10000000
	neg	r1,	r1		// r1 = 0xf0000000
	add	#-1,	r1		// r1 = 0xefffffff
	and	r1,	r0
	ldc	r0,	sr
*/

	mov.l	.L_PTEH, r0
#ifndef VM_DISABLE
	mov.l	.L_pager_load_page, r1
#endif
	stc	spc,	r5
	jsr	@r1
	 mov.l	@r0,	r4
/*
	// Block exception
	stc	sr,	r0
	mov.l	.L_SR_BL,	r1	// r1 = 0x10000000
	or	r1,	r0
	ldc	r0,	sr
*/
	lds.l	@r15+,	pr
	lds.l	@r15+,	mach
	lds.l	@r15+,	macl
	mov.l	@r15+,	r14
	mov.l	@r15+,	r13
	mov.l	@r15+,	r12
	mov.l	@r15+,	r11
	mov.l	@r15+,	r10
	mov.l	@r15+,	r9
	mov.l	@r15+,	r8
	mov.l	@r15+,	r7
	mov.l	@r15+,	r6
	mov.l	@r15+,	r5
	mov.l	@r15+,	r4
	mov.l	@r15+,	r3
	mov.l	@r15+,	r2
	mov.l	@r15+,	r1
	rte
	 mov.l	@r15+,	r0
	.align	2
.L_PTEH:
	.long	0xff000000
#ifndef VM_DISABLE
.L_pager_load_page:
	.long	_pager_load_page
#endif
.L_SR_IMASK:
	.long	0xf0
.L_SR_BL:
	.long	0x10000000
