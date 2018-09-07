
/*-
 * Copyright (c) 2008-2009, 2011 UCHIYAMA Yasushi.  All rights reserved.
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
#include <stdlib.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <sys/board.h>

#include <sys/srec.h>
#include <vector_virtual.h>

SHELL_COMMAND_DECL (l);

void
board_main (uint32_t arg __attribute__((unused)))
{

  // Install ROM monitor interrupt handler.
  vector_table_init ();

  shell_command_register (&l_cmd);

  iprintf ("H8 Simple Monitor Build %s %s\n", __DATE__, __TIME__);

  // Make shell of this thread.
  shell_prompt (stdin, stdout);
  /* NOTREACHED */
}

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE;
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{
}

uint32_t
l (int32_t argc __attribute__((unused)),
   const char *argv[] __attribute__((unused)))
{
  int s;
  int32_t sz;
  srec_return_t ret;
  uint32_t err = -1;

  iprintf ("Send S-record file.\n");
  s = intr_suspend ();
  srec_init ();
  ret = srec_load (&sz);

  iprintf ("Read %ld byte. %s\n", sz, srec_error_message[ret]);
  if (ret != SREC_EOF)
    goto failed;
  //  iprintf ("Start address: 0x%x\n", srec_start_addr);

  // Change to loaded program's vector table.
  vector_table_update (VIRTUAL_VECTOR_TABLE_ADDR, TRUE, TRUE);
  iprintf ("->%p\n", (void *)srec_start_addr);
  // call user program.
  __asm volatile ("jsr @%0" :: "r" (srec_start_addr));
  iprintf ("done.\n");
  err = 0;

 failed:
  intr_resume (s);

  return err;
}
