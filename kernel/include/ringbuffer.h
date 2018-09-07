
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

#ifndef _RINGBUFFER_H_
#define	_RINGBUFFER_H_
#include <sys/monitor.h>
// monitor locked ringbuffer. for thread context.
typedef struct ringbuffer
{
  struct monitor mon;

  size_t input;
  size_t output;
  size_t size;	// must be power of 2
  size_t free;
  bool event_hooked;	// for continuation.
  uint8_t buffer[0];
} *ringbuffer_t;

#define	RINGBUFFER_SIZE(x)	(sizeof (struct ringbuffer) + (x))

__BEGIN_DECLS
ringbuffer_t ringbuffer_init (uint8_t *, size_t);
void ringbuffer_write (ringbuffer_t, uint8_t *, size_t);
size_t ringbuffer_read (ringbuffer_t, uint8_t *, size_t);
size_t ringbuffer_read_ext (ringbuffer_t, uint8_t *, size_t,
			    continuation_func_t);
__END_DECLS

#endif
