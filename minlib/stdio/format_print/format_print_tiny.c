
/*-
 * Copyright (c) 2008-2011 UCHIYAMA Yasushi.  All rights reserved.
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

#ifndef SELFTEST
#include <sys/system.h>
#include <sys/console.h>
#else
#define	STATIC
#include <sys/types.h>
#endif

STATIC void print_digit (uint32_t, int, void (*)(void *, int8_t), void *);


void
format_print (struct _stream_char_ops *iop, const char *fmt, va_list ap)
{
  char *p;
  int32_t i;
  uint32_t u;
  void (*_putc)(void *, int8_t) = iop->putc;
  void *ctx = iop->ctx;

  while (/*CONSTCOND*/1)
    {
      while ((i = *fmt++) != '%')
	{
	  if (i == '\0')
	    return;
	  _putc (ctx, i);
	}

      switch (i = *fmt++)
	{
	case 's':
	  p = va_arg (ap, char *);
	  while ((i = *p++))
	    _putc (ctx, i);
	  break;

	case 'c':
	  i = va_arg(ap, int);
	  _putc (ctx, i & 0x7f);
	  break;

	case 'd':
	  u = va_arg (ap, int);
	  if ((int)u < 0)
	    {
	      _putc (ctx, '-');
	      u = -(int)u;
	    }
	  print_digit (u, 10, _putc, ctx);
	  break;

	case 'x':
	  u = va_arg (ap, uint32_t);
	  print_digit (u, 16, _putc, ctx);
	  break;

	default:
	  _putc (ctx, '%');
	  if (i == '\0')
	    return;
	  _putc (ctx, i);
	  break;
	}
    }
}

void
print_digit (uint32_t u, int base, void (*_putc)(void *, int8_t), void *ctx)
{
  char *p, buf[16]; // INTMAX = 2147483647. enuf to store.
  const char hexdigits[] = "0123456789abcdef";

  p = buf;

  do
    {
      char q = hexdigits[u % base];
      *p++ = q;
    }
  while (u /= base);

  do
    _putc (ctx, *--p);
  while (p > buf);
}

