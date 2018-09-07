
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
#include <sys/stream.h>

#include <stdio.h>
#include <stdarg.h>

#define	NSTATIC_FILE	4

STATIC struct _file __file[NSTATIC_FILE];
struct _file *stdin;
struct _file *stdout;
struct _file *altin;
struct _file *altout;

struct _file *_file_allocate ()
{
  struct _file *f;
  size_t i;

  for (i = 0, f = __file; i < sizeof __file / sizeof (__file[0]); i++, f++)
    {
      if (!f->active)
	{
	  f->active = TRUE;
	  return f;
	}
    }

  return NULL;
}

void
_file_deallocate (struct _file *f)
{

  f->active = FALSE;
}

void
_boot_console_init (struct _stream_char_ops *ops)
{

  md_uart_putc1 ('S');
  altin = _file_allocate ();
  altout = _file_allocate ();
  stdin = altin;
  stdout = altout;

  altin->char_ops = ops;
  altout->char_ops = ops;

  if (altin)
    md_uart_putc1 ('I');
  if (altout)
    md_uart_putc1 ('O');

  altout->char_ops->putc (0, '@');
  altout->char_ops->putc (0, '\n');
}



