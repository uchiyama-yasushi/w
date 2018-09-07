
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

#include <sys/types.h>
#include <sys/board.h>
#include <sys/shell.h>

#include <stdio.h>

SHELL_COMMAND_DECL (lua_test);

void
board_main (uint32_t arg __attribute__((unused)))
{

  printf ("hello world pc=%p sp=%p\n", cpu_get_pc (), cpu_get_sp ());

  printf ("sizeof short :%d\n", sizeof (short));
  printf ("sizeof int :%d\n", sizeof (int));
  printf ("sizeof long :%d\n", sizeof (long));
  printf ("sizeof long long:%d\n", sizeof (long long));

  SHELL_COMMAND_REGISTER (lua_test);

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

uint32_t
lua_test (int32_t argc, const char *argv[])
{
  int main (int argc, const char *argv[]);

  main (argc, argv);
  return 0;
}

void
board_reset (uint32_t cause)
{
  cause=cause;
  while (/*CONSTCOND*/1)
    ;
  //NOTREACHED
}

void stack_check (const char *s) { s=s;}
