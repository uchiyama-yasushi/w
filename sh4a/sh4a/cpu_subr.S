
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
FUNC (_exception_disable)
	mov.l	1f,	r0
	stc	sr,	r1
	or	r1,	r0
	ldc	r0,	sr
	rts
	 nop
	.align	2
1:	.long	0x10000000

FUNC (_exception_enable)
	mov.l	1f,	r0
	stc	sr,	r1
	and	r1,	r0
	ldc	r0,	sr
	rts
	 nop
	.align	2
1:	.long	0xefffffff

FUNC (_intr_enable)
	mov.l	2f,	r0
	stc	sr,	r1
	and	r0,	r1
	ldc	r1,	sr
	rts
	 nop

FUNC (_intr_disable)
	mov.l	1f,	r0
	stc	sr,	r1
	or	r0,	r1
	ldc	r1,	sr
	rts
	 nop

FUNC (_intr_suspend)
	mov.l	1f,	r2
	stc	sr,	r1
	mov	r1,	r0
	or	r2,	r1
	ldc	r1,	sr
	rts
	 and	r2,	r0

FUNC (_intr_resume)
	mov.l	2f,	r2
	stc	sr,	r0
	and	r2,	r0
	or	r4,	r0
	ldc	r0,	sr
	rts
	 nop

FUNC (_cpu_sleep)
	sleep
	rts
	 nop

FUNC (_cpu_reset)
	mov.l	reset_vector, r0
	jmp	@r0
	 nop

FUNC (_cpu_get_pc)
	sts	pr,	r0
	rts
	 add	#-2,	r0

FUNC (_cpu_run_P2)
	sts	pr,	r0
	mov.l	.L_P2,	r1
	or	r1,	r0
	lds	r0,	pr
	rts
	 nop
.L_P2:	.long	0x20000000

FUNC (_cpu_run_P1)
	sts	pr,	r0
	mov.l	.L_P1,	r1
	and	r1,	r0
	lds	r0,	pr
	rts
	 nop
.L_P1:	.long	0xdfffffff

	.align	2
1:	.long	0x000000f0
2:	.long	0xffffff0f
reset_vector:
	.long	0xa0000000