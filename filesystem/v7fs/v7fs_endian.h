/*	$NetBSD$	*/

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

#ifndef _V7FS_ENDIAN_H_
#define	_V7FS_ENDIAN_H_

struct v7fs_self;
__BEGIN_DECLS;
v7fs_daddr_t val24_normal_order_read (uint8_t *);
void val24_normal_order_write (v7fs_daddr_t, uint8_t *);
__END_DECLS
#ifdef V7FS_EI
#define	V7FS_VAL32(x, v)	((*(x)->val._32) (v))
#define	V7FS_VAL16(x, v)	((*(x)->val._16) (v))
#define	V7FS_VAL24_READ(x, a)	((*(x)->val._24read) (a))
#define	V7FS_VAL24_WRITE(x, v, a) ((*(x)->val._24write) (v, a))
void v7fs_endian_init (struct v7fs_self *, int);
#else
#define	V7FS_VAL32(x, v)	(v)
#define	V7FS_VAL16(x, v)	(v)
#define	V7FS_VAL24_READ(x, a)	(val24_normal_order_read (a))
#define	V7FS_VAL24_WRITE(x, v, a) (val24_normal_order_write (v, a))
#endif //V7FS_EI
#endif //!_V7FS_ENDIAN_H_
