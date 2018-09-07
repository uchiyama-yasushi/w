
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

#include <sys/console.h>

BOOT_CONSOLE_DECL (userland);

int sys_write(int, const void *, int);
int sys_read(int, void *, int);

void
boot_console_init ()
{

  _boot_console_init (&userland_console_ops);
}

int8_t
userland_getc (void *arg)
{
  arg=arg;
  int8_t c;
  int i;

  while ((i = sys_read (0, &c, 1)) <= 0)
    ;
  if (c == '\n')
    c = '\r';

  return c;
}

int8_t
userland_getc_cont (void *arg, continuation_func_t cont __attribute__((unused)))
{

  return userland_getc (arg);
}

void
userland_putc (void *arg __attribute__((unused)), int8_t c)
{

  md_uart_putc1 (c);
}

int8_t
userland_ungetc (void *arg, int8_t c)
{
  struct userland_boot_console *cons = (struct userland_boot_console *)arg;

  if (c == EOF)
    return EOF;

  cons->save = c;

  return c;
}

void
md_uart_putc1 (int8_t c)
{

  sys_write (1, &c, 1);
}

