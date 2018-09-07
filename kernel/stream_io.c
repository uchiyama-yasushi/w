
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


#include <sys/system.h>
#include <string.h>

#include <io.h>

STATIC void io_recv_thread (uint32_t);
STATIC void io_send_thread (uint32_t);
STATIC void io_thread_start (struct _io_thread *, void (*)(uint32_t), uint32_t,
			     const char *);
STATIC continuation_func io_recv_loop __attribute__((noreturn));
STATIC continuation_func io_send_loop __attribute__((noreturn));

void
io_init (struct _io_buf *iob, struct _io_device *device)
{

  // Attach device specific routines.
  iob->device = device;

  device->disable ();

  // ring buffer for interrupt handler <-> receive thread.
  iob->intr_recv_buf = rbuf_nolock_init (iob->intr_buf, IO_FIFO_SIZE);

  io_thread_start (&iob->recv, io_recv_thread, (uint32_t)iob, "io recv");
  io_thread_start (&iob->send, io_send_thread, (uint32_t)iob, "io send");

  // Set parameter to machine-dependent part.
  device->init (iob);

  thread_priority (iob->recv.th, PRI_HIGH); // this is interrupt thread.
  thread_priority (iob->send.th, PRI_HIGH);

  device->enable ();
}

void
io_thread_start (struct _io_thread *ioth, void (*func)(uint32_t), uint32_t arg,
		 const char *name)
{
  memset (ioth, 0, sizeof (struct _io_thread));

  ioth->rb = ringbuffer_init (ioth->buf, IO_FIFO_SIZE);
  ioth->th = thread_create_no_stack (&ioth->tc, name, func, arg);
  ioth->parent = current_thread;
  thread_start (ioth->th);

  // make sure that ringbuffer_read is waiting.
  while (!ioth->started)
    thread_block (NULL);
}

void
io_recv_thread (uint32_t arg)
{
  struct _io_buf *iob = (struct _io_buf *)arg;
  iob->recv.started = TRUE;
  current_thread->continuation_arg = arg;
  intr_enable ();
  thread_wakeup (iob->recv.parent);

  io_recv_loop ();
  // NOTREACHED
}

void
io_recv_loop ()	// stackless thread.
{
  // When wakeup, new stack is allocated here.
  struct _io_buf *iob = (struct _io_buf *)current_thread->continuation_arg;
  uint8_t buf[IO_FIFO_SIZE];
  size_t sz;

  // Read data pushed by device's interrupt handler.
  intr_disable ();
  sz = rbuf_nolock_read (iob->intr_recv_buf, buf, IO_FIFO_SIZE);
  intr_enable ();

  // Put data to reader thread.
  if (sz)
    ringbuffer_write (iob->recv.rb, buf, sz);

  thread_block (io_recv_loop); // Here current stack is discarded.
  assert (0);
  // NOTREACHED
  while (/*CONSTCOND*/1)
    ;
}

void
io_send_thread (uint32_t arg)
{
  struct _io_buf *iob = (struct _io_buf *)arg;
  iob->send.started = TRUE;
  current_thread->continuation_arg = arg;

  intr_enable ();
  thread_wakeup (iob->send.parent);

  io_send_loop ();
  // NOTREACHED
}

void
io_send_loop ()	// stackless thread
{
  struct _io_buf *iob = (struct _io_buf *)current_thread->continuation_arg;
  uint8_t buf[IO_FIFO_SIZE];
  size_t i;

  intr_enable ();
  while (/*CONSTCOND*/1)
    {
      // If no data is available, blocked. and will wakeup on function start.
      size_t sz = ringbuffer_read_ext (iob->send.rb, buf,
				       IO_FIFO_SIZE, io_send_loop);
      for (i = 0; i < sz; i++)
	iob->device->putc (buf[i]);
    }
}

/* API */
void
io_putc (struct _io_buf *iob, int8_t c)
{

  ringbuffer_write (iob->send.rb, (uint8_t *)&c, 1);
}

int8_t
io_getc_cont (struct _io_buf *iob, continuation_func_t cont)
{
  uint8_t c;

  if (ringbuffer_read_ext (iob->recv.rb, &c, 1, cont) == 1)
    return c;

  return 0;
}

int8_t
io_getc (struct _io_buf *iob)
{
  uint8_t c;

  if (ringbuffer_read (iob->recv.rb, &c, 1) == 1)
    return c;

  return 0;
}

int8_t
io_ungetc (struct _io_buf *iob, int8_t c)
{

  if (c == EOF)
    return EOF;

  ringbuffer_writeback (iob->recv.rb, (uint8_t *)&c, 1);

  return c;
}
