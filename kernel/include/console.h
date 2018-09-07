
/*-
 * Copyright (c) 2008, 2011 UCHIYAMA Yasushi.  All rights reserved.
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

#ifndef _CONSOLE_H_
#define	_CONSOLE_H_
#include <stdio.h>

__BEGIN_DECLS
// for interrupt context or no threading support.
#define	BOOT_CONSOLE_DECL(x)					\
STATIC int8_t x ## _getc (void *);				\
STATIC int8_t x ## _getc_cont (void *, continuation_func_t);	\
STATIC void x ## _putc (void *, int8_t);			\
STATIC int8_t x ## _ungetc (void *, int8_t);			\
								\
STATIC struct x ## _boot_console				\
{								\
  char save;							\
} x ## _boot_console;						\
								\
STATIC struct _stream_char_ops x ## _console_ops =		\
{								\
  (void *)&x ## _boot_console,					\
  x ## _getc,							\
  x ## _putc,							\
  x ## _ungetc,							\
  x ## _getc_cont						\
};

#define	CONSOLE_OUT_DECL(x)					\
STATIC void x ## _putc (void *, int8_t);			\
								\
STATIC struct _stream_char_ops x ## _console_ops =		\
{								\
  NULL,								\
  NULL,								\
  x ## _putc,							\
  NULL,								\
  NULL,								\
};

// bufferd console. need thread support.
void console_init (bool);
// disable buffered console. // for debug use.
void console_polling (void);
// boot console helper routine.
void _boot_console_init (struct _stream_char_ops *);
struct _file *_file_allocate (void);
void _file_deallocate (struct _file *);

// Machine depenedent implementation.
#ifndef THREAD_DISABLE
#include <sys/thread.h>
#include <sys/rbuf_nolock.h>
void md_uart_init (thread_t, rbuf_nolock_t);
void md_uart_intr_enable (void);
void md_uart_intr_disable (void);
#endif //!THREAD_DISABLE
void md_uart_putc1 (int8_t);

__END_DECLS
#endif //_CONSOLE_H_
