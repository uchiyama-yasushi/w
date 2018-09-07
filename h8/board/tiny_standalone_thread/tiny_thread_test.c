
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
#include <sys/console.h>
#include <sys/thread.h>
#include <sys/delay.h>
#include <sys/timer.h>
#include <sys/shell.h>

#include <reg.h>
#include <sys/console.h>
#include <stdlib.h>

#include <sys/board.h>
#include <sys/srec.h>

SHELL_COMMAND_DECL (test);
struct thread_control app_tc;

void display_thread (uint32_t);
__BEGIN_DECLS
void timer_a_start (void);
__END_DECLS


uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | DELAY_CALIBRATE | BUFFERED_CONSOLE_ENABLE;
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{
  printf ("%s\n", __FUNCTION__);
}

void
board_main (uint32_t arg __attribute__((unused)))
{

  shell_command_register (&test_cmd);
#if 0
  // Application thread.
  thread_t app_th = thread_create ((uint8_t *)&app_tc, 0, "display",
				   display_thread, 0);
  thread_start (app_th);
#endif
  printf ("%s\n", __FUNCTION__);
  // Make shell of this thread.
  timer_a_start ();

  shell_prompt (stdin, stdout);
  /* NOTREACHED */
}

void
display_thread (uint32_t arg __attribute__((unused)))
{
  int i;

  while (/*CONSTCOND*/1)
    {
      printf ("application thread\n");
      for (i = 0; i < 100; i++)
	timer_sleep (10000);/* 10msec */
    }
  // NOTREACHED
}

uint32_t
test (int32_t argc __attribute__((unused)), const char *argv[] __attribute__((unused)))
{

  printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$test$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

  return 0;
}
