
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

/* These data are overwrited by jmp instruction on execution time */
/* ROM vector point these. */
#include <asm/machine.h>
	H8ARCH
	.section .vector_link_table
	.globl _jmp07
_jmp07: .long	0
	.globl _jmp08
_jmp08: .long	0
	.globl _jmp09
_jmp09: .long	0
	.globl _jmp10
_jmp10: .long	0
	.globl _jmp11
_jmp11: .long	0
	.globl _jmp12
_jmp12: .long	0
	.globl _jmp13
_jmp13: .long	0
	.globl _jmp14
_jmp14: .long	0
	.globl _jmp15
_jmp15: .long	0
	.globl _jmp16
_jmp16: .long	0
	.globl _jmp17
_jmp17: .long	0
	.globl _jmp18
_jmp18: .long	0
	.globl _jmp19
_jmp19: .long	0
	.globl _jmp20
_jmp20: .long	0
	.globl _jmp21
_jmp21: .long	0
	.globl _jmp22
_jmp22: .long	0
	.globl _jmp23
_jmp23: .long	0
	.globl _jmp24
_jmp24: .long	0
	.globl _jmp25
_jmp25: .long	0
