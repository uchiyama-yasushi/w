
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

#ifndef _H8SXWRITE_H_
#define	_H8SXWRITE_H_

#ifdef DEBUG
#define	DPRINTF(fmt, args...)	printf(fmt, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

#define	VPRINTF(fmt, args...)	if (verbose) printf(fmt, ##args)

#define	ROUND(x, a)	((((x) + ((a) - 1)) & ~((a) - 1)))
#define	TRUNC(x, a)	((x) & ~((a) - 1))

typedef uint32_t addr_t;	// H8SX address space is 4GB.

typedef int bool;
#define	FALSE	0
#define	TRUE	1

#define	STATIC	static

#if BYTE_ORDER == LITTLE_ENDIAN
#define	GET16(x)	bswap16(x)
#define	GET32(x)	bswap32(x)
#define	SET16(x)	bswap16(x)
#define	SET32(x)	bswap32(x)
#elif BYTE_ORDER == BIG_ENDIAN
#define	GET16(x)	(x)
#define	GET32(x)	(x)
#define	SET16(x)	(x)
#define	SET32(x)	(x)
#else
#error	"Unsupported BYTE_ORDER"
#endif

// Board specific configuration.
struct board_config
{
  uint8_t MD_CLK;	// 0:8-18MHz input(pull-down), 1:16MHz input(pull-up).
  uint8_t input_frequency;	// MHz
};

struct minmax
{
  uint32_t min;
  uint32_t max;
};

struct flash_config
{
  char device_code[4];
  int n_clock_range;
  struct minmax *clock_range;
  int n_clock_mode;
  uint8_t *clock_mode;

  int n_userboot_mat;
  struct minmax *userboot_mat;
  int n_user_mat;
  struct minmax *user_mat;

  uint8_t n_mult_type;
  size_t memory_size;
  size_t write_size;
};

bool h8sx_bootprog_init (int, struct board_config *, struct flash_config *);
void h8sx_bootprog_fini (struct flash_config *);
bool h8sx_bootprog_write (struct flash_config *, uint8_t *, size_t);
bool srec_to_loadimage (uint8_t *, size_t, uint8_t *, size_t);

extern bool verbose;

#endif	// !_H8SXWRITE_H_
