
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

#ifndef _TYPES_H_
#define	_TYPES_H_
#ifndef __WOS__
#include <sys/types.h>
typedef int bool;
#define	TRUE	1
#define	FALSE	0
#define	NULL	0
#else //!__WOS__

typedef	signed char		 int8_t;
typedef	unsigned char		uint8_t;
typedef	short int		int16_t;
typedef	unsigned short int     uint16_t;
#ifdef __amd64__
typedef	int			int32_t;
typedef	unsigned int	       uint32_t;
#else
typedef	long			int32_t;
typedef	unsigned long	       uint32_t;
#endif
typedef long long		int64_t;
typedef unsigned long long      uint64_t;

#ifndef __cplusplus
typedef int8_t			bool;
#endif //!__cplusplus
#define	TRUE	1
#define	FALSE	0
#define	NULL	0
#define	NBBY	8

typedef	uint32_t	daddr_t;
typedef uint32_t	off_t;
typedef uint16_t	ino_t;

#define	DEV_BSIZE	512

#ifdef __cplusplus
#define	__BEGIN_DECLS		extern "C" {
#define	__END_DECLS		}
#else //__cplusplus
#define	__BEGIN_DECLS
#define	__END_DECLS
#endif //__cplusplus

#include <machine/types.h>
typedef	saddr_t			ptrdiff_t;

#include <sys/endian.h>
#endif //!__WOS__
#endif //!_TYPES_H_
