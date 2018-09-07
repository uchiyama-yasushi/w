
/*-
 * Copyright (c) 2010 UCHIYAMA Yasushi.  All rights reserved.
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
#include <sys/shell.h>
#include <sys/board.h>
#include <reg.h>
#include <sys/debug.h>

#include <stdlib.h>

#include <console_machdep.h> //XXX
#include <sys/thread.h>
#include <setjmp.h>

SHELL_COMMAND_DECL (test);

void irq3_init (void);

void
board_main (uint32_t arg __attribute__((unused)))
{

  SHELL_COMMAND_REGISTER (test);

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

#include <cpu.h>

uint32_t
test (int32_t argc __attribute__((unused)),
   const char *argv[] __attribute__((unused)))
{
  jmp_buf env;
  int i;

  printf ("hello world\n");
  printf ("%10s\n", "ohayo");
  printf ("%-10s\n", "ohayo");
  printf ("%-10.3s\n", "ohayo");
  printf ("%10.4s\n", "ohayo");

  printf ("%x %x %d %d %d\n", 0xac1dcafe, 0x1badface, 14, 28, 99);
  printf ("%x\n", 0xdeadbeaf);
  printf ("%d\n", 123456);
  printf ("%c\n", 'n');
  printf ("%s\n", "ohayo");
  printf ("%08x\n", 0x34);
  printf ("%8x\n", 0x34);
  printf ("%#08x\n", 0x34);
  printf ("%#-8x\n", 0x34);
  printf ("%#010x\n", 0x34);
  printf ("%x\n", 0x34);
  printf ("%d\n", 45);
  printf ("%d\n", -45);
  printf ("%8d\n", -45);
  printf ("%-8d\n", -45);


  if (!(i = setjmp (env)))
    {
      printf ("setjmp\n");
    }
  else
    {
      printf ("longjmp (12345)%d\n", i);
      return 0;
    }
  longjmp (env, 12345);
  printf ("XXX longjmp failed.\n");

  return 0;
}
