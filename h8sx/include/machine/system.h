
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

#ifndef _MACHINE_SYSTEM_H_
#define	_MACHINE_SYSTEM_H_
#include <sys/types.h>
#define	BYTE_ORDER		BIG_ENDIAN
#define	H8_INTR_CONTROL_MODE	2

__BEGIN_DECLS
void intr_enable (void);
void intr_disable (void);
cpu_status_t intr_suspend (void);// return old value
cpu_status_t intr_resume (cpu_status_t);// return current value
// Optional
cpu_status_t intr_status (void);// return current value
#if H8_INTR_CONTROL_MODE != 0
cpu_status_t intr_priority (cpu_status_t);// return old value
#endif

__END_DECLS

#if H8_INTR_CONTROL_MODE == 0 || H8_INTR_CONTROL_MODE == 1 || \
  H8_INTR_CONTROL_MODE == 3
// Wait 2 state for the new value becomes valid.
#define	intr_enable()	__asm volatile ("andc.b	#0x7f, ccr\nnop\nnop")
#define	intr_disable()	__asm volatile ("orc.b	#0x80, ccr\nnop\nnop")
#elif H8_INTR_CONTROL_MODE == 2
#define	intr_enable()	__asm volatile ("andc.b	#0xf8, exr\nnop\nnop")
#define	intr_disable()	__asm volatile ("orc.b	#0x07, exr\nnop\nnop")
#else
#error "specify interrupt control mode."
#endif

#define	userbit_set()	((void)0)
#define	userbit_get(x)	((void)0)

#endif
