
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
#include <stdio.h>
#include <stdlib.h>

#include <sys/board.h>
#include <sys/srec.h>

SHELL_COMMAND_DECL (D);
struct thread_control app_tc __attribute__ ((aligned (4)));

void display_thread (uint32_t);
void display (void);
void ioport_init (void);
void prepare_digit_buffer_led (const char *);
void poll_switch (void);

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | DELAY_CALIBRATE | BUFFERED_CONSOLE_ENABLE;
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{

  ioport_init ();
}

void
board_main (uint32_t arg __attribute__((unused)))
{

  shell_command_register (&D_cmd);

  // Application thread.
  thread_t app_th = thread_create_no_stack (&app_tc, "display",
					    display_thread, 0);
  thread_start (app_th);

  //  shell_set_device (SERIAL, SERIAL, FALSE);	// Don't echo back.
  shell_prompt (stdin, NULL);
  // NOTREACHED
}

void
display_thread (uint32_t arg __attribute__((unused)))
{
  thread_priority (current_thread, PRI_HIGH);	// highest
  while (/*CONSTCOND*/1)
    {
      display ();
      poll_switch ();
    }
  // NOTREACHED
}

uint32_t
D (int32_t argc, const char *argv[])
{

  if (argc < 2)
    return -1;

  prepare_digit_buffer_led (argv[1]);

  return 0;
}
