
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
#include <sys/shell.h>

#include <reg.h>
#include <sys/board.h>

#include "led_display.h"
#include "keyboard16.h"
#include "caliper_manager.h"

uint8_t app_tls[THREAD_STACK_SIZE (THREAD_STACKSIZE_DEFAULT)]
__attribute ((aligned (4)));

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE;
  //  return 0; // no serial output.
}

void
board_main (uint32_t arg __attribute__((unused)))
{
  thread_t app_th;

  // 7-seg LED display unit.
  led_display_init ();

  // 16-button keyboard.
  keyboard_init ();

  // Application thread.
  app_th = thread_create_with_stack (app_tls, THREAD_STACKSIZE_DEFAULT, "app0",
			  caliper_manager, 0);
  thread_start (app_th);
  // Wait until application thread is started.
  while (!caliper_manager_thread)
    thread_rotate_ready_queue ();

  thread_priority (current_thread, PRI_APPLO);
  // 7seg-LED dynamic display loop.
  //  shell_prompt (SERIAL);

  intr_enable ();
  while (/*CONSTCOND*/1)
    {
      led_display_output ();
    }
  /* NOTREACHED */
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{
  // notghing to do.
}

