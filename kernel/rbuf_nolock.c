
/*-
 * Copyright (c) 2008 UCHIYAMA Yasushi.  All rights reserved.
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
#include <string.h>

#include <sys/console.h>
#include <sys/rbuf_nolock.h>

rbuf_nolock_t
rbuf_nolock_init (uint8_t *buf, size_t size)
{
  struct rbuf_nolock *rb = (struct rbuf_nolock *)buf;

  memset (rb, 0, sizeof (struct rbuf_nolock));

  rb->input = 0;
  rb->output = 0;
  rb->size = size;
  rb->free = size;

  return rb;
}

size_t
rbuf_nolock_write (rbuf_nolock_t rb, uint8_t *buf, size_t size)
{
  size_t i, j, sz;

  if (rb->free == 0)
    return 0;

  sz = rb->free < size ? rb->free : size;
  for (i = 0; i < sz; i++)
    {
      j = (rb->input + i) & (rb->size -1);
      rb->buffer[j] = *buf++;
    }

  rb->input += sz;
  rb->input &= (rb->size - 1);
  rb->free -= sz;

  return sz;
}

size_t
rbuf_nolock_read (rbuf_nolock_t rb, uint8_t *buf, size_t size)
{
  size_t buffered, sz, i, j;

  if ((rb->free - rb->size) == 0)
    return 0;

  buffered = rb->size - rb->free;

  sz = buffered > size ? size : buffered;
  for (i = 0; i < sz; i++)
    {
      j = (rb->output + i) & (rb->size -1);
      buf[i] = rb->buffer[j];
    }

  rb->output += sz;
  rb->output &= (rb->size - 1);
  rb->free += sz;

  return sz;
}
