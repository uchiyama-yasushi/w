
/*-
 * Copyright (c) 2008-2009 UCHIYAMA Yasushi.  All rights reserved.
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
#include <sys/thread.h>
#include <sys/delay.h>
#include <sys/shell.h>
#include <sys/prompt.h>

#include <reg.h>
#include <sys/board.h>

#include <libud01/ud01dev.h>
#include <sys/srec.h>
#include <vector_virtual.h>

SHELL_COMMAND_DECL (l);
SHELL_COMMAND_DECL (g);
SHELL_COMMAND_DECL (kill);
SHELL_COMMAND_DECL (v);

STATIC bool vector_table_override;

void
board_main (uint32_t arg)
{
  arg = arg;
  delay_calibrate ();

  // link ROM vector table to RAM vector table.
  vector_table_init (); // ROM monitor myself.

  shell_command_register (&l_cmd);
  shell_command_register (&g_cmd);
  shell_command_register (&v_cmd);
  shell_command_register (&kill_cmd);

  iprintf ("H8/3052 ud01 Monitor Build %s %s\n", __DATE__, __TIME__);
  iprintf ("[ VIRTUAL VECTOR TABLE *%s* ]\n", vector_table_override ?
	   "ON" : "OFF");
  iprintf (" OFF: overlay module, ON: standalone module.\n");
  // Make shell of this thread.

  shell_prompt (stdin, stdout);
  /* NOTREACHED */
}

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | BUFFERED_CONSOLE_ENABLE | RAM_CHECK_ENABLE |
    DELAY_CALIBRATE;
}

uint32_t
l (int32_t argc __attribute__((unused)),
   const char *argv[] __attribute__((unused)))
{
  int32_t sz;
  srec_return_t ret;

  cpu_status_t s = intr_suspend ();
  md_uart_intr_disable ();
  srec_init ();
  ret = srec_load (&sz);

  iprintf ("Read %ld byte. %s\n", sz, srec_error_message[ret]);
  if (ret != SREC_EOF)
    goto failed;
  // Change to loaded profgram's vector table.
  vector_table_update (VIRTUAL_VECTOR_TABLE_ADDR, vector_table_override, FALSE);

  iprintf ("Start address: %p\n", (void *)srec_start_addr);

  // call user program.
  __asm volatile ("jsr @%0" :: "r" (srec_start_addr));

 failed:
  md_uart_intr_enable ();
  intr_resume (s);
  return 0;
}

uint32_t
g (int32_t argc __attribute__((unused)),
   const char *argv[] __attribute__((unused)))
{

  if (!srec_start_addr)
    return -1;

  intr_disable ();
  __asm volatile ("jmp @%0" :: "r" (srec_start_addr));
  /* NOTREACHED */

  return 0;
}

uint32_t
v (int32_t argc __attribute__((unused)),
   const char *argv[] __attribute__((unused)))
{

  vector_table_override ^= 1;

  printf ("[ VIRTUAL VECTOR TABLE %s ]\n", vector_table_override ? "ON" : "OFF");

  return 0;
}


uint32_t
kill (int32_t argc, const char *argv[])
{
  thread_id_t th;

  if (argc < 2)
    return -1;
  th = atoi (argv[1]);

  cpu_status_t s = intr_suspend ();
  thread_destroy (th);
  // Change to monitor vector table.
  vector_table_init ();
  intr_resume (s);

  return 0;
}
