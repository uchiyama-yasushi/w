
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


#include <sys/system.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <sys/board.h>
#include <reg.h>
#include <sys/debug.h>

#include <stdlib.h>

#include <console_machdep.h> //XXX
#include <sys/thread.h>
#include <setjmp.h>

SHELL_COMMAND_DECL (test);

void
board_main (uint32_t arg __attribute__((unused)))
{

  SHELL_COMMAND_REGISTER (test);

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

#include <sys/types.h>
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

#define	bswap32pdp_hostle(x)				\
  ((uint32_t)						\
   ((((x) & 0xff000000) >> 16) |			\
    (((x) & 0x00ff0000) >> 16) |			\
    (((x) & 0x0000ff00) << 16) |			\
    (((x) & 0x000000ff) << 16)))

static
int16_t
extract16(unsigned char *s)
{
	return (s[0] << 0) | (s[1] << 8);
}


int32_t extract24(unsigned char *s)
{
	return (s[1] << 0) | (s[2] << 8) | (s[0] << 16);
}


static int32_t extract32(unsigned char *s)
{
	return (s[2] << 0) | (s[3] << 8) | (s[0] << 16) | (s[1] << 24);
}

uint32_t
test (int32_t argc __attribute__((unused)),
   const char *argv[] __attribute__((unused)))
{
  char buf[64];

  sprintf (buf, "ohayo %d", 32);
  printf ("%s\n", buf);

  int n = snprintf (buf, 16, "ohayo %d", 32);
  printf ("%d, %s\n", n, buf);

  uint32_t a = 0xac1dcafe;
  uint16_t b = 0x1bad;
  uint8_t *p = (uint8_t *)&a;
  int i;
  for (i = 0; i < 4; i++)
    printf ("%p:%x\n", p + i, p[i]);

  printf ("%x %x %x\n", a, extract32 ((uint8_t *)&a), bswap32pdp_hostle(a));

  printf ("%x %x\n", extract16 ((uint8_t *)&b), bswap16(b));


  return 0;
}
