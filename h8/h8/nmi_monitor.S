
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
/* NMI handler for rommonitor. */
/* XXX Advanced mode only XXX */
	.h8300h
	.section .text
	.text

	.global _nmi
_nmi:
	mov.l	sp, @_nmi_sp,
	mov.l	#_stack_start,	sp
	mov.l	er0,	@-sp
	mov.l	er1,	@-sp
	mov.l	er2,	@-sp
	mov.l	er3,	@-sp
	mov.l	er4,	@-sp
	mov.l	er5,	@-sp
	mov.l	er6,	@-sp
	mov.l	@_nmi_sp, er0
	mov.l	er0,	@-sp

	jsr	@_nmi_dump_info

	adds	#4,	sp
	mov.l	@sp+	er6
	mov.l	@sp+	er5
	mov.l	@sp+	er4
	mov.l	@sp+	er3
	mov.l	@sp+	er2
	mov.l	@sp+	er1
	mov.l	@sp+	er0
	mov.l	@_nmi_sp, sp
	rte

	.section .data
	.globl _nmi_sp
_nmi_sp:
	.long	0
