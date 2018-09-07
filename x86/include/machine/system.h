
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

#ifndef _MACHINE_SYSTEM_H_
#define	_MACHINE_SYSTEM_H_
#include <sys/types.h>
#define	BYTE_ORDER	LITTLE_ENDIAN

// kernel interface. must be implemented.
#define	cpu_sleep() while (/*CONSTCOND*/1)__asm volatile ("hlt")

#define	intr_enable()	__asm volatile ("sti");
#define	intr_disable()	__asm volatile ("cli");

// Using ID bit. (21)
#define	userbit_set() __asm volatile ("pushf\n			\
					orl $(1 << 21), (%esp)\n\
					popf")
#define	userbit_clear() __asm volatile ("pushf\n			\
					andl $~(1 << 21),(%esp)\n	\
					popf")
#define	userbit_get(c) __asm volatile ("push %%eax\n		\
					pushf\n			\
					movl (%%esp), %%eax\n	\
					popf\n			\
					shr $21, %%eax\n	\
					movb %%al, %0\n		\
					pop %%eax" : "=d"(c):)

__BEGIN_DECLS
void panic (const char *) __attribute__ ((noreturn));
cpu_status_t intr_suspend (void);
void intr_resume (cpu_status_t) __attribute__((regparm(1)));;
__END_DECLS
#endif
