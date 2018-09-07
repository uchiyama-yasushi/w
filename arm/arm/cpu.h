
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

#ifndef _ARM_REG_H_
#define	_ARM_REG_H_
// Status register.
#define	PSR_N		0x80000000	// Negative
#define	PSR_Z		0x40000000	// Zero
#define	PSR_C		0x20000000	// Carry
#define	PSR_V		0x10000000	// oVerflow
#define	PSR_Q		0x08000000	// overflow (DSP extention)
#define	PSR_J		0x01000000	// JAVA (Jzzelle) mode
#define	PSR_I		0x00000080	// IRQ disable
#define	PSR_F		0x00000040	// FIQ disable
#define	PSR_T		0x00000020	// Thumb mode
#define	PSR_MMASK	0x0000001f
#define	PSR_MUSR	0x10
#define	PSR_MFIQ	0x11
#define	PSR_MIRQ	0x12
#define	PSR_MSVC	0x13
#define	PSR_MABT	0x17
#define	PSR_MUND	0x1b
#define	PSR_MSYS	0x1f

__BEGIN_DECLS
void cpu_psr_dump (uint32_t);
void cpu_regs_save (uint32_t *);
void cpu_regs_load (uint32_t *);

uint32_t cpu_psr_get (void);
cpu_status_t intr_status (void);

__END_DECLS

#endif
