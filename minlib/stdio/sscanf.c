
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

#include <minlib.h>
#include <sys/types.h>
#include <sys/stream.h>
#include <sys/stream_mem.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int
LIBFUNC (sscanf) (const char *str, const char *fmt, ...)
{
  struct _stream_char_ops ops;
  struct _stream_mem_buf memb;
  va_list ap;

  stream_mem_init (&memb, (uint8_t *)str, strlen (str) + 1, &ops);

  va_start (ap, fmt);
  int ret = format_scan (&ops, fmt, ap);
  va_end (ap);

  return ret;
}

#ifdef SELFTEST
//cc -DSELFTEST -g sscanf.c ../kernel/stream_mem.c ../kernel/format_scan.c
main ()
{
  char s[] = "ohayo 3 0xac1dcafe 1BADFACE I 013";
  //  char s[] = "ohayo 3 ac1dcafe I";
  char c;
  char p[16];
  int i, j, k, l;

  my_sscanf (s, "%s %d %x %x %c %o", p, &i, &j, &l, &c, &k);
  printf ("%s %d %x %x %c %d\n", p, i, j, l, c, k);
  sscanf (s, "%s %d %x %x %c %o", p, &i, &j, &l, &c, &k);
  printf ("%s %d %x %x %c %d\n", p, i, j, l, c, k);

  while (1)
    {
      my_scanf ("%s %d %x %x %c %o", p, &i, &j, &l, &c, &k);
      printf ("%s %d %x %x %c %d\n", p, i, j, l, c, k);
    }
}
#endif
