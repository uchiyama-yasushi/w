
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
// SH2A 7262 I/O port

#ifndef _SH2A_7262_IOPORT_H_
#define	_SH2A_7262_IOPORT_H_

// Port G
/* Control Register */
#define	PGCR7	((volatile uint16_t *)0xfffe38c0)
#define	PGCR6	((volatile uint16_t *)0xfffe38c2)
#define	PGCR5	((volatile uint16_t *)0xfffe38c4)
#define	PGCR4	((volatile uint16_t *)0xfffe38c6)
#define	PGCR3	((volatile uint16_t *)0xfffe38c8)
#define	PGCR2	((volatile uint16_t *)0xfffe38ca)
#define	PGCR1	((volatile uint16_t *)0xfffe38cc)
#define	PGCR0	((volatile uint16_t *)0xfffe38ce)
/* I/O Register (R/W) port direction. 1:output, 0:input */
#define	PGIOR1	((volatile uint16_t *)0xffffe38d)
#define	PGIOR0	((volatile uint16_t *)0xffffe38d)
/* Data Register (R/W) */
#define	PGDR1	((volatile uint16_t *)0xfffe38d4)
#define	PGDR0	((volatile uint16_t *)0xfffe38d6)
/* Port Register (R) pin status.*/
#define	PGPR1	((volatile uint16_t *)0xfffe38d8)
#define	PGPR0	((volatile uint16_t *)0xfffe38da)


#define	BSET(bit, addr)								\
  __asm volatile ("bset.b %0, @(0, %1)":: "i"(bit), "r"(addr): "memory")
#define	BCLR(bit, addr)								\
  __asm volatile ("bclr.b %0, @(0, %1)":: "i"(bit), "r"(addr): "memory")

#define	SWAP16(r)								\
  ({										\
	uint16_t _tmp_;								\
	__asm volatile ("swap.b %1, %0": "=r"(_tmp_) : "r"(r));			\
	_tmp_;									\
  })
#define	SWAP32(r)								\
  ({										\
	uint32_t _tmp_;								\
	__asm volatile ("swap.b %1, %0\n\t"					\
			"swap.w %0, %0\n\t"					\
			"swap.b %0, %0\n\t"					\
			: "=r"(_tmp_) : "r"(r));				\
	_tmp_;									\
  })

#define	CLIPS_B(r)								\
  ({										\
	int32_t _tmp_  = r;							\
	__asm volatile ("clips.b %0" : "+r"(_tmp_));				\
	_tmp_;									\
  })
#define	CLIPU_B(r)								\
  ({										\
	uint32_t _tmp_  = r;							\
	__asm volatile ("clipu.b %0" : "+r"(_tmp_));				\
	_tmp_;									\
  })
#define	CLIPS_W(r)								\
  ({										\
	int32_t _tmp_  = r;							\
	__asm volatile ("clips.w %0" : "+r"(_tmp_));				\
	_tmp_;									\
  })
#define	CLIPU_W(r)								\
  ({										\
	uint32_t _tmp_  = r;							\
	__asm volatile ("clipu.w %0" : "+r"(_tmp_));				\
	_tmp_;									\
  })

#endif


