
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
#include <sys/stream_mem.h>
#include <stdio.h>

STATIC void stream_mem_putc (void *, int8_t);
STATIC int8_t stream_mem_getc (void *);
STATIC int8_t stream_mem_ungetc (void *, int8_t);
//STATIC char *stream_mem_gets (void *, char *, int);
//STATIC int stream_mem_puts (void *, const char *);

void
stream_mem_init (struct _stream_mem_buf *memb, uint8_t *buf, size_t sz,
		 struct _stream_char_ops *cops)
{
  memb->base = memb->ptr = buf;
  memb->end = buf + sz;
  memb->save = 0;

  cops->ctx = memb;
  cops->getc = stream_mem_getc;
  cops->putc = stream_mem_putc;
  cops->ungetc = stream_mem_ungetc;
}

void
stream_mem_putc (void *arg, int8_t c)
{
  struct _stream_mem_buf *memb = (struct _stream_mem_buf *)arg;

  //XXX limit EOF
  if (memb->ptr < memb->end)
    *memb->ptr++ = c;

}

int8_t
stream_mem_getc (void *arg)
{
  struct _stream_mem_buf *memb = (struct _stream_mem_buf *)arg;
  char c;

  if ((c = memb->save))
    {
      memb->save = 0;
      return c;
    }

  return (memb->ptr < memb->end) ? *memb->ptr++ : EOF;
}

int8_t
stream_mem_ungetc (void *arg, int8_t c)
{
  struct _stream_mem_buf *memb = (struct _stream_mem_buf *)arg;

  if (c == EOF)
    return EOF;

  memb->save = c;

  return c;
}
#if 0
char *
stream_mem_gets (void *arg, char *buf, int sz)
{
  struct _stream_mem_buf *memb = (struct _stream_mem_buf *)arg;
  int i;
  char *p;
  char c;

  for (i = 0, p = buf; i < sz - 1; i++, memb->ptr++)
    {
      if (!(memb->ptr < memb->end))
	{
	  if (i == 0)
	    return 0;
	}
      if ((c = *memb->ptr) != '\n')
	*p++ = c;
    }
  *p = '\0';

  return buf;
}

int
stream_mem_puts (void *arg, const char *s)
{
  struct _stream_mem_buf *memb = (struct _stream_mem_buf *)arg;
  char c;

  while ((c = *s++))
    {
      if (!(memb->ptr < memb->end))
	return EOF;
      *memb->ptr++ = c;
    }
  return 0;
}
#endif
