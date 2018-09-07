
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

#ifndef _CPU_H_
#define	_CPU_H_
#define	TRA	((volatile uint32_t *)0xff000020)
#define	EXPEVT	((volatile uint32_t *)0xff000024)
#define	INTEVT	((volatile uint32_t *)0xff000028)

#define	SR_MD	0x40000000	// Mode: user(0)/privilege(1)
#define	SR_RB	0x20000000	// Register Bank 0:0, 1:1
#define	SR_BL	0x10000000	// Exception/Interrupt block(1)
#define	SR_FD	0x00008000	// FPU Disable(1)
#define	SR_M	0x00000200	// DIV0S,DIV0U,DIV1
#define	SR_Q	0x00000100	// DIV0S DIV0U,DIV1
#define	SR_S	0x00000002	// MAC instruction
#define	SR_T	0x00000001	// True/False
#define	SR_IMASK_MASK	0xf
#define	SR_IMASK_SHIFT	4

#define	CPU_GET_SR(r)  __asm volatile ("stc	sr, %0" : "=r" (r))
#define	CPU_GET_SSR(r)  __asm volatile ("stc	ssr, %0" : "=r" (r))
#define	CPU_GET_SPC(r)  __asm volatile ("stc	spc, %0" : "=r" (r))
#define	CPU_GET_GBR(r)  __asm volatile ("stc	gbr, %0" : "=r" (r))
#define	CPU_GET_DBR(r)  __asm volatile ("stc	dbr, %0" : "=r" (r))
#define	CPU_GET_VBR(r)  __asm volatile ("stc	vbr, %0" : "=r" (r))
#define	CPU_GET_PC(r)					\
{							\
  register uint32_t r0 __asm ("r0");			\
  __asm volatile ("mova @(0, pc), %0" : "=r"(r0));	\
  (r) = r0 - 2/*or 4 XXX MOVA address calcuration is 4byte align*/;	\
}

#define	CPU_SET_SR(r)  __asm volatile ("ldc	%0, sr" :: "r" (r))
#define	CPU_SET_SSR(r)  __asm volatile ("ldc	%0, ssr" :: "r" (r))
#define	CPU_SET_SPC(r)  __asm volatile ("ldc	%0, spc" :: "r" (r))
#define	CPU_SET_GBR(r)  __asm volatile ("ldc	%0, gbr" :: "r" (r))
#define	CPU_SET_DBR(r)  __asm volatile ("ldc	%0, dbr" :: "r" (r))
#define	CPU_SET_VBR(r)  __asm volatile ("ldc	%0, vbr" :: "r" (r))

#define	CPU_SYNC_RESOURCE() __asm volatile ("icbi @%0" :: "r"(0x89000000))


__BEGIN_DECLS
#ifdef DEBUG
void cpu_dump_sr (uint32_t);
#endif
void cpu_init (uint32_t/*Hz*/, int);
void cpu_info (void);
void cpu_run_P2 (void);
void cpu_run_P1 (void);
uint32_t cpu_get_pc (void);
extern uint32_t cpu_input_clock;
extern int cpu_mode;

__END_DECLS
#endif
