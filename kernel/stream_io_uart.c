
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
#include <console_machdep.h>
#include <io.h>

STATIC struct _io_buf _buffer;
STATIC void uart_init (struct _io_buf *);
STATIC void uart_buf_putc (int8_t);
STATIC int8_t uart_buf_getc (continuation_func_t);

struct _io_device _buffer_device =
  { uart_init, md_uart_intr_enable, md_uart_intr_disable, md_uart_putc1 };

void
console_init (bool on)
{
  if (!on)
    return;

  // Install buffered putc/getc.
  console_putc_install (SERIAL, BUFFERED, uart_buf_putc);
  console_getc_install (SERIAL, BUFFERED, uart_buf_getc);
  io_init (&_buffer, _buffer_device);
}

void
uart_init (struct _io_buf *iob)
{

  md_uart_init (iob->recv.th, iob->intr_recv_buf);
}

void
uart_buf_putc (int8_t c)
{
  uint8_t cr = '\r';

  if (c == '\n')
    io_putc (_buffer, '\r');
  io_putc (_buffer, c);
}

int8_t
uart_buf_getc (continuation_func_t cont)
{

  return io_getc_cont (_buffer, cont);
}

