
/*-
 * Copyright (c) 2009, 2011 UCHIYAMA Yasushi.  All rights reserved.
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
#include <sys/types.h>
#include <sys/console.h>
#include <pcat/bios.h>

BOOT_CONSOLE_DECL (com);
BOOT_CONSOLE_DECL (bios);

void
bios_console_init (enum console_device device)
{

  if (device == SERIAL)
    {
      call16 (bios_com_init, 0, 0);
      _boot_console_init (&com_console_ops);
    }
  else
    {
      // BIOS Text.
      _boot_console_init (&bios_console_ops);
    }
}

// Serial
void
md_uart_putc1 (int8_t c)	// place holder.
{
  c = c;
}

void
com_putc (void *arg __attribute__((unused)), int8_t c)
{

  call16 (bios_com_putc, c, 0);
}

int8_t
com_getc (void *arg)
{
  struct com_boot_console *cons = (struct com_boot_console *)arg;
  char c;

  if ((c = cons->save))
    {
      cons->save = 0;
      return c;
    }

  return call16 (bios_com_getc, 0, 0);
}

int8_t
com_getc_cont (void *arg, continuation_func_t cont __attribute__((unused)))
{

  return com_getc (arg);
}

// Text.
void
bios_putc (void *arg __attribute__((unused)), int8_t c)
{

  if (c == '\n')
    call16 (bios_vga_putc, '\r', 0);
  call16 (bios_vga_putc, c, 0);
}

int8_t
bios_getc (void *arg)
{
  struct bios_boot_console *cons = (struct bios_boot_console *)arg;
  char c;

  if ((c = cons->save))
    {
      cons->save = 0;
      return c;
    }

  return call16 (bios_kbd_getc, 0, 0);
}

int8_t
bios_getc_cont (void *arg, continuation_func_t cont __attribute__((unused)))
{

  return bios_getc (arg);
}

int8_t
com_ungetc (void *arg, int8_t c)
{
  struct com_boot_console *cons = (struct com_boot_console *)arg;

  if (c == EOF)
    return EOF;

  cons->save = c;

  return c;
}

int8_t
bios_ungetc (void *arg, int8_t c)
{
  struct bios_boot_console *cons = (struct bios_boot_console *)arg;

  if (c == EOF)
    return EOF;

  cons->save = c;

  return c;
}


