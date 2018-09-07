
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
#include <sys/thread.h>
#include <sys/monitor.h>

#include <sys/console.h>
#include <sys/ringbuffer.h>

#define	RINGBUFFER_FULL		0x1
#define	RINGBUFFER_EMPTY	0x2

#ifdef RINGBUFFER_DEBUG
#define	DPRINTF(fmt, args...)	iprintf(fmt, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

STATIC size_t __read_subr (ringbuffer_t, uint8_t *, size_t);

ringbuffer_t
ringbuffer_init (uint8_t *buf, size_t size)
{
  struct ringbuffer *rb = (struct ringbuffer *)buf;

  memset (rb, 0, sizeof (struct ringbuffer));
  monitor_init (&rb->mon, "ringbuffer");

  rb->input = 0;
  rb->output = 0;
  rb->size = size;
  rb->free = size;

  return rb;
}

void
ringbuffer_write (ringbuffer_t rb, uint8_t *buf, size_t size)
{
  monitor_t mon = &rb->mon;
  size_t i, j, sz;

  while (size > 0)
    {
      monitor_enter (mon);
      DPRINTF ("W< i(%d) o(%d) f(%d) s(%d) remain=%d\n",
	       rb->input, rb->output, rb->free, rb->size, size);
      while (rb->free == 0)
	{
	  DPRINTF ("W:full!\n");
	  monitor_wait (mon, RINGBUFFER_FULL);
	  DPRINTF ("W:get! free=%d\n", rb->free);
	}

      sz = rb->free < size ? rb->free : size;
      DPRINTF ("move====>%d %d %d\n", sz, rb->free, size);
      for (i = 0; i < sz; i++)
	{
	  j = (rb->input + i) & (rb->size -1);
	  assert (j < rb->size);
	  rb->buffer[j] = *buf++;
	}

      rb->input += sz;
      rb->input &= (rb->size - 1);
      rb->free -= sz;
      size -= sz;
      monitor_signal (mon, RINGBUFFER_EMPTY);
      monitor_exit (mon);
      DPRINTF ("W> i(%d) o(%d) f(%d) s(%d)\n", rb->input, rb->output,
	       rb->free, rb->size);
    }
}

size_t
ringbuffer_read (ringbuffer_t rb, uint8_t *buf, size_t size)
{
  monitor_t mon = &rb->mon;

  monitor_enter (mon);
  DPRINTF ("\t\t\t\tR< i(%d) o(%d) f(%d) s(%d)\n", rb->input, rb->output,
	   rb->free, rb->size);

  while (rb->free == rb->size)
    {
      DPRINTF ("\t\t\t\tR:empty!\n");
      monitor_wait (mon, RINGBUFFER_EMPTY);
    }

  return __read_subr (rb, buf, size);
}

size_t
ringbuffer_read_ext (ringbuffer_t rb, uint8_t *buf, size_t size,
		     continuation_func_t cont)
{
  monitor_t mon = &rb->mon;

  monitor_enter (mon);
  DPRINTF ("\t\t\t\tR< i(%d) o(%d) f(%d) s(%d)\n", rb->input, rb->output,
	   rb->free, rb->size);

  if (rb->free == rb->size)
    {
      DPRINTF ("\t\t\t\tR:empty!\n");
      if (!rb->event_hooked)
	{
	  rb->event_hooked = TRUE;
	  monitor_event_hook (mon, RINGBUFFER_EMPTY);
	}
      //      monitor_exit (mon);
      thread_block (cont);
    }
  if (rb->event_hooked)
    {
      //      monitor_enter (mon);
      rb->event_hooked = FALSE;
      monitor_event_unhook (mon);
    }

  return __read_subr (rb, buf, size);
}

size_t
__read_subr (ringbuffer_t rb, uint8_t *buf, size_t size)
{
  monitor_t mon = &rb->mon;
  size_t buffered, sz, i, j;

  buffered = rb->size - rb->free;

  sz = buffered > size ? size : buffered;
  DPRINTF ("\t\t\t\tR:filled! ==>%d %d %d %d\n", sz, buffered, size, rb->free);
  for (i = 0; i < sz; i++)
    {
      j = (rb->output + i) & (rb->size -1);
      assert (j < rb->size);
      buf[i] = rb->buffer[j];
    }

  rb->output += sz;
  rb->output &= (rb->size - 1);
  rb->free += sz;
  DPRINTF ("\t\t\t\tR> i(%d) o(%d) f(%d) s(%d)\n", rb->input, rb->output,
		   rb->free, rb->size);
  monitor_signal (mon, RINGBUFFER_FULL);
  monitor_exit (mon);

  return sz;
}
