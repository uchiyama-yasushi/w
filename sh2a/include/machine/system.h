
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

#define	cpu_sr_get()								\
  ({										\
    uint32_t _tmp_;								\
    __asm volatile ("stc sr, %0" : "=r"(_tmp_));				\
    _tmp_;									\
  })


#define	intr_enable()								\
  ({										\
    uint32_t _tmp0_, _tmp1_;							\
    __asm volatile ("movi20	#-241,	%0\n\t"					\
		    "stc	sr,	%1\n\t"					\
		    "and	%0,	%1\n\t"					\
		    "ldc	%1,	sr"					\
		    : "=r"(_tmp0_), "=r"(_tmp1_));				\
  })
#define	intr_disable()								\
  ({										\
    uint32_t _tmp0_, _tmp1_;							\
    __asm volatile ("movi20	#0xf0,	%0\n\t"					\
		    "stc	sr,	%1\n\t"					\
		    "or		%0,	%1\n\t"					\
		    "ldc	%1,	sr" : "=r"(_tmp0_), "=r"(_tmp1_));	\
  });

#define	intr_suspend()								\
  ({										\
    uint32_t _tmp0_, _tmp1_, _tmp2_;						\
    __asm volatile ("movi20	#0xf0,	%0\n\t"					\
		    "stc	sr,	%1\n\t"					\
		    "mov	%1,	%2\n\t"					\
		    "or		%0,	%1\n\t"					\
		    "ldc	%1,	sr\n\t"					\
		    "and	%0,	%2"					\
		    : "=r"(_tmp0_), "=r"(_tmp1_), "=r"(_tmp2_));		\
    _tmp2_;/* Return old mask */						\
  })
#define	intr_resume(s)								\
  ({										\
    uint32_t _tmp0_, _tmp1_;							\
    __asm volatile ("movi20	#-241,	%0\n\t"					\
		    "stc	sr,	%1\n\t"					\
		    "and	%0,	%1\n\t"					\
		    "or		%2,	%1\n\t"					\
		    "ldc	%1,	sr\n\t"					\
		    "not	%0,	%0\n\t"					\
		    "and	%0,	%1\n\t"					\
		    : "=r"(_tmp0_), "=r"(_tmp1_) : "r"(s));			\
    _tmp1_;/* Return old mask */						\
  })

// Optional
#define	intr_status()								\
  ({										\
    uint32_t _tmp0_, _tmp1_;							\
    __asm volatile ("movi20	#0xf0,	%0\n\t"					\
		    "stc	sr,	%1\n\t"					\
		    "and	%0,	%1\n\t"					\
		    : "=r"(_tmp0_), "=r"(_tmp1_));				\
    _tmp1_;									\
  })

__BEGIN_DECLS
cpu_status_t intr_priority (cpu_status_t);
__END_DECLS

#define	userbit_set()	((void)0)
#define	userbit_get(x)	((void)0)
#endif
