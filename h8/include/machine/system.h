
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

#ifndef _MACHINE_SYSTEM_H_
#define	_MACHINE_SYSTEM_H_
#include <sys/types.h>

#define	BYTE_ORDER	BIG_ENDIAN

__BEGIN_DECLS
void intr_enable (void);
void intr_disable (void);
cpu_status_t intr_suspend (void);
void intr_resume (cpu_status_t);
__END_DECLS

#define	intr_enable()	__asm volatile ("andc.b	#0x7f, ccr\nnop\nnop")
#define	intr_disable()	__asm volatile ("orc.b	#0x80, ccr\nnop\nnop")

// assert()/ NMI uses.
#define	userbit_set()	__asm volatile ("orc #0x10, ccr\n")
#define	userbit_clear()	__asm volatile ("andc #0xef, ccr\n")
#define	userbit_get(c)	__asm volatile ("stc.b ccr, %0l" : "=r"(c):)

static inline void cpu_sleep() { __asm volatile ("sleep"); }

__BEGIN_DECLS
// currently unused.
extern volatile uint16_t *__sys_flag;
extern inline void sys_flag_set () { *__sys_flag = 1; }
extern inline bool sys_flag_get () { return *__sys_flag; }
extern inline void sys_flag_clear () { *__sys_flag = 0; }
__END_DECLS

#endif
