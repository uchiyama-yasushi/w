
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

#ifndef _STREAM_IO_H_
#define	_STREAM_IO_H_
#include <sys/thread.h>
#include <sys/ringbuffer.h>
#include <sys/rbuf_nolock.h>

#define	BUFSIZ	1024		// setbuf, setvbuf buffer size.
#define	EOF	(-1)
#define	IO_FIFO_SIZE		32

/* Device buffer */
struct _io_buf;

struct _io_thread	// stackless thread
{
  struct thread_control tc;
  uint8_t buf[RINGBUFFER_SIZE (IO_FIFO_SIZE)];
  ringbuffer_t rb;
  thread_t th;
  thread_t parent;
  bool started;
} __attribute ((aligned (4)));

struct _io_device
{
  void (*init) (struct _io_buf *);
  // Mainly enable/disable device interrupt.
  void (*enable) (void);
  void (*disable) (void);
  void (*putc) (int8_t);
};

struct _io_buf
{
  struct _io_thread send;
  struct _io_thread recv;
  // ring buffer for interrupt handler <-> receive thread.
  rbuf_nolock_t intr_recv_buf;
  uint8_t intr_buf[RBUF_NOLOCK_SIZE (IO_FIFO_SIZE)];
  struct _io_device *device;
};

void io_init (struct _io_buf *, struct _io_device *);
void io_putc (struct _io_buf *, int8_t);
int8_t io_getc (struct _io_buf *);
int8_t io_getc_cont (struct _io_buf *, continuation_func_t);
int8_t io_ungetc (struct _io_buf *, int8_t);

#endif	//!_STREAM_IO_H_
