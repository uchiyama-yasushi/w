
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

#include <sys/types.h>
#include <sys/shell.h>
#include <sys/board.h>

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
  uint32_t pc;

  pc = cpu_get_pc ();
  printf ("ohayo0 %x\n", cpu_get_pc ());
  cpu_run_P2 ();
  pc = cpu_get_pc ();
  printf ("ohayo1 %x\n", pc);
  cpu_run_P1 ();
  pc = cpu_get_pc ();
  printf ("ohayo2 %x\n", pc);

  return 0;
}