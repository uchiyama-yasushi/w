
/*-
 * Copyright (c) 2011 UCHIYAMA Yasushi.  All rights reserved.
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

#ifndef _ENDIAN_H_
#define	_ENDIAN_H_
#define	LITTLE_ENDIAN	1234
#define	BIG_ENDIAN	4321
#define	PDP_ENDIAN	3412

#include <machine/endian.h>

#define	bswap32(x)					\
  ((uint32_t)						\
   ((((x) & 0xff000000) >> 24) |			\
    (((x) & 0x00ff0000) >>  8) |			\
    (((x) & 0x0000ff00) <<  8) |			\
    (((x) & 0x000000ff) << 24)))

#define	bswap16(x)					\
  ((uint16_t)						\
   ((((x) & 0xff00) >> 8) |				\
    (((x) & 0x00ff) << 8)))

#define	bswap32pdp(x)					\
  ((uint32_t)						\
   ((((x) & 0xff000000) >> 16) |			\
    (((x) & 0x00ff0000) >> 16) |			\
    (((x) & 0x0000ff00) << 16) |			\
    (((x) & 0x000000ff) << 16)))

#endif	// !_ENDIAN_H_
