
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

/* AKIZUKI SG12232 122x32 graphic LCD (SUNLIKE) */
/* CAN NOT EXPORT */

#include <sys/system.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/console.h>

#include <libud01/sg12232.h>

CONSOLE_OUT_DECL (sg12232);
STATIC struct _file sg12232out;

struct _file *
lcd_init ()
{

  lcd_write (LCD_CMD_BOTH, CMD_RESET);
  lcd_write (LCD_CMD_BOTH, CMD_DISPLAY_ON | DISPLAY_ON);
  lcd_write (LCD_CMD_BOTH, CMD_START_LINE | 0);

  lcd_clear (0x00);

  sg12232out.char_ops = &sg12232_console_ops;

  return &sg12232out;
}

void
sg12232_putc (void *ctx __attribute__((unused)), int8_t c)
{

  lcd_putc (c);
}
