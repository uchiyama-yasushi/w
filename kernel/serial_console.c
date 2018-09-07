
/*-
 * Copyright (c) 2008-2009 UCHIYAMA Yasushi.  All rights reserved.
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

// Bufferd serial console.

#include <sys/system.h>
#include <string.h>

#include <sys/delay.h>
#include <sys/timer.h>
#include <sys/thread.h>
#include <sys/ringbuffer.h>
#include <sys/rbuf_nolock.h>

#include <sys/console.h>
#include <console_machdep.h>

#ifndef UART_FIFO_SIZE
#define	UART_FIFO_SIZE	32
#endif

#ifndef CONSOLE_STACK_SIZE
#define	CONSOLE_STACK_SIZE	THREAD_STACKSIZE_DEFAULT
#endif

#define	USE_CONTINUATION

struct uart_thread
{
#ifdef USE_CONTINUATION
  struct thread_control tc;
#else
  uint8_t tls[THREAD_STACK_SIZE (CONSOLE_STACK_SIZE)];
#endif
  uint8_t buf[RINGBUFFER_SIZE (UART_FIFO_SIZE)];
  ringbuffer_t rb;
  thread_t th;
  bool started;
}
  uart_recv __attribute ((aligned (4))),
  uart_send __attribute ((aligned (4)));

STATIC void uart_recv_thread (uint32_t);
STATIC void uart_send_thread (uint32_t);
STATIC void uart_thread_start (struct uart_thread *, void (*)(uint32_t),
			      const char *);
// buffered I/O
STATIC void uart_buf_putc (void *, int8_t);
STATIC int8_t uart_buf_getc_cont (void *, continuation_func_t);
STATIC int8_t uart_buf_getc (void *);
STATIC int8_t uart_buf_ungetc (void *, int8_t);
STATIC thread_t parent_thread;

STATIC uint8_t __ibuf[RBUF_NOLOCK_SIZE (UART_FIFO_SIZE)];
STATIC rbuf_nolock_t uart_recv_buf;

STATIC struct buffered_console
{
  char save;
} buffered_console;

STATIC struct _stream_char_ops buffered_console_ops =
{
  (void *)&buffered_console, uart_buf_getc, uart_buf_putc, uart_buf_ungetc,
  uart_buf_getc_cont,
};

void
console_init (bool on)
{

  if (!on)
    return;
  md_uart_intr_disable ();

  // ring buffer for interrupt handler <-> receive thread.
  uart_recv_buf = rbuf_nolock_init (__ibuf, UART_FIFO_SIZE);

  // buffered console. need threading support.

  uart_thread_start (&uart_recv, uart_recv_thread, "uart recv");

  uart_thread_start (&uart_send, uart_send_thread, "uart send");

  // Set parameter to machine-dependent part.
  md_uart_init (uart_recv.th, uart_recv_buf);

  thread_priority (uart_recv.th, PRI_HIGH); // this is interrupt thread.

  thread_priority (uart_send.th, PRI_HIGH);

  // Install buffered putc/getc.
  if (stdin == altin || !stdin)
    stdin = _file_allocate ();
  if (stdout == altout || !stdout)
    stdout = _file_allocate ();

  stdin->char_ops = &buffered_console_ops;
  stdout->char_ops = &buffered_console_ops;

  md_uart_intr_enable ();
}

void
console_polling ()
{
  cpu_status_t s = intr_suspend ();
  stdin = altin;
  stdout = altout;
  md_uart_intr_disable ();
  intr_resume (s);
}

void
uart_thread_start (struct uart_thread *st, void (*func)(uint32_t),
		  const char *name)
{
  memset (st, 0, sizeof (struct uart_thread));

  st->rb = ringbuffer_init (st->buf, UART_FIFO_SIZE);
#ifdef USE_CONTINUATION
  st->th = thread_create_no_stack (&st->tc, name, func, (uint32_t)(addr_t)st);
#else
  st->th = thread_create_with_stack (st->tls, CONSOLE_STACK_SIZE, name, func,
			  (uint32_t)(addr_t)st);
#endif
  thread_start (st->th);

  // make sure that ringbuffer_read is waiting.
  parent_thread = current_thread;

  while (!st->started)
    thread_block (NULL);

}

struct uart_thread *__st_recv;
struct uart_thread *__st_send;
continuation_func recv_loop __attribute__((noreturn));
continuation_func send_loop __attribute__((noreturn));

void
uart_recv_thread (uint32_t arg)
{
#ifdef USE_CONTINUATION
  __st_recv = (struct uart_thread *)(addr_t)arg;
  __st_recv->started = TRUE;
#else
  struct uart_thread *st = (struct uart_thread *)(addr_t)arg;
  uint8_t buf[UART_FIFO_SIZE];
  size_t sz;
  cpu_status_t s;
  st->started = TRUE;
#endif
  intr_enable ();
  thread_wakeup (parent_thread);
#ifdef USE_CONTINUATION
  recv_loop ();
#else
  while (/*CONSTCOND*/1)
    {
      thread_block (NULL);	// Wait interrupt.

      s = intr_suspend ();
      sz = rbuf_nolock_read (uart_recv_buf, buf, UART_FIFO_SIZE);
      intr_resume (s);

      if (!sz)
	continue;

      ringbuffer_write (st->rb, buf, sz);
    }
#endif
  // NOTREACHED
}
#ifdef USE_CONTINUATION
void
recv_loop ()
{
  uint8_t buf[UART_FIFO_SIZE];
  size_t sz;

  intr_disable ();
  sz = rbuf_nolock_read (uart_recv_buf, buf, UART_FIFO_SIZE);
  intr_enable ();

  if (sz)
    ringbuffer_write (__st_recv->rb, buf, sz);

  thread_block (recv_loop);
  assert (0);
  // NOTREACHED
  while (/*CONSTCOND*/1)
    ;
}

void
send_loop ()
{
  uint8_t buf[UART_FIFO_SIZE];
  size_t i;

  intr_enable ();
  while (/*CONSTCOND*/1)
    {
      size_t sz = ringbuffer_read_ext (__st_send->rb, buf, UART_FIFO_SIZE, send_loop);

      for (i = 0; i < sz; i++)
	md_uart_putc1 (buf[i]);
    }
}
#endif

void
uart_send_thread (uint32_t arg)
{
#ifdef USE_CONTINUATION
  __st_send = (struct uart_thread *)(addr_t)arg;
  __st_send->started = TRUE;
#else
  struct uart_thread *st = (struct uart_thread *)(addr_t)arg;
  uint8_t buf[UART_FIFO_SIZE];
  size_t i;
  st->started = TRUE;
#endif
  intr_enable ();
  thread_wakeup (parent_thread);
#ifdef USE_CONTINUATION
  send_loop ();
#else

  while (/*CONSTCOND*/1)
    {
      size_t sz = ringbuffer_read (st->rb, buf, UART_FIFO_SIZE);

      for (i = 0; i < sz; i++)
	md_uart_putc1 (buf[i]);
    }
#endif
  // NOTREACHED
}

void
uart_buf_putc (void *arg __attribute__((unused)), int8_t c)
{
  uint8_t cr = '\r';

  if (c == '\n')
    ringbuffer_write (uart_send.rb, &cr, 1);
  ringbuffer_write (uart_send.rb, (uint8_t *)&c, 1);
}

int8_t
uart_buf_getc_cont (void *arg, continuation_func_t cont)
{
  struct buffered_console *cons = (struct buffered_console *)arg;
  uint8_t c;

  if ((c = cons->save))
    {
      cons->save = 0;
      return c;
    }

  if (ringbuffer_read_ext (uart_recv.rb, &c, 1, cont) == 1)
    return c;

  return 0;
}

int8_t
uart_buf_getc (void *arg)
{
  struct buffered_console *cons = (struct buffered_console *)arg;
  uint8_t c;

  if ((c = cons->save))
    {
      cons->save = 0;
      return c;
    }

  if (ringbuffer_read (uart_recv.rb, &c, 1) == 1)
    return c;

  return 0;
}

int8_t
uart_buf_ungetc (void *arg, int8_t c)
{
  struct buffered_console *cons = (struct buffered_console *)arg;

  if (c == EOF)
    return EOF;

  cons->save = c;

  return c;
}
