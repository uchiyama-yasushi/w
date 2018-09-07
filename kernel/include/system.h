
/*-
 * Copyright (c) 2008-2009, 2011 UCHIYAMA Yasushi.  All rights reserved.
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

#ifndef _SYSTEM_H_
#define	_SYSTEM_H_
#include <sys/types.h>
#include <sys/debug.h>
#include <machine/system.h>
#include <sys/endian.h>
#include <errno.h>

#define	ROUND(x, a)	((((x) + ((a) - 1)) & ~((a) - 1)))
#define	TRUNC(x, a)	((x) & ~((a) - 1))
#define	ALIGN_CHECK(x, a)	assert (((x) & ((a) - 1)) == 0)

#define	sign(x)				\
  ({					\
    typeof (x) a = (x);			\
    a == 0 ? 0 : a < 0 ? -1 : 1;	\
  })

__BEGIN_DECLS
bool check_ram (uint32_t start, uint32_t end, bool verbose);
void os_panic (const char *) __attribute__((noreturn));
__END_DECLS

typedef void (continuation_func)(void);
typedef continuation_func *continuation_func_t __attribute__((noreturn));
#endif	// !_SYSTEM_H_
