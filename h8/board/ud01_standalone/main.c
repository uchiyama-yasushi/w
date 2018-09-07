
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
#include <sys/types.h>

#include <sys/console.h>
#include <sys/shell.h>
#include <sys/board.h>
#include <stdlib.h>

void
board_main (uint32_t arg __attribute__((unused)))
{
  uint32_t r;

  __asm volatile ("mov sp, %0" : "=r"(r));
  printf ("sp=%lx\n", r);
  console_polling ();
  __asm volatile ("mov sp, %0" : "=r"(r));
  printf ("sp=%lx\n", r);
  clearerr (stdin);
  __asm volatile ("mov sp, %0" : "=r"(r));
  printf ("sp=%lx\n", r);
  printf ("%x %x %d %d %d\n", 0x80000, 0x80000, 1024, 0, 1);
  __asm volatile ("mov sp, %0" : "=r"(r));
  printf ("sp=%lx\n", r);

  printf ("%x %lx %d %d %ld\n", 0x80000, 0x80000, 1024, 0, (long)1);
  __asm volatile ("mov sp, %0" : "=r"(r));
  printf ("sp=%lx\n", r);


  shell_prompt (stdin, stdout);
}

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | BUFFERED_CONSOLE_ENABLE | DELAY_CALIBRATE;
}
