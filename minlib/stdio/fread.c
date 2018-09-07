
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
#include <stdio.h>

size_t
LIBFUNC (fread) (void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  printf ("%s\n", __FUNCTION__);
  int i = 0;

  if (!stream->binary_ops)
    {
      int8_t (*_getc)(void *) = stream->char_ops->getc;
      void *ctx = stream->char_ops->ctx;
      char *p = (char *)ptr;
      size *= nmemb;
      for (i = 0; size--; i++, p++)
	{
	  int c;
	  if ((c = _getc (ctx)) == 4)//XXX ^D
	    {
	      stream->flags |= _IOEOF;
	      break;
	    }
	  *p = c;
	}
    }
  printf ("fread:%d\n", i);

  return i;
}
