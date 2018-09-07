
/*-
 * Copyright (c) 2009 UCHIYAMA Yasushi.  All rights reserved.
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
#include <cpu.h>
#include <reg.h>

#include <string.h>
#include <sys/thread.h>
#include <sys/delay.h>
#include <sys/timer.h>


SHELL_COMMAND_DECL (test);

thread_func app0_thread;
thread_t app_th;
struct thread_control app0_tc;

void
board_main (uint32_t arg)
{
  printf ("%s: arg=%x\n", __FUNCTION__, arg);
  shell_command_register (&test_cmd);

  app_th = thread_create_no_stack (&app0_tc, "app0", app0_thread, 0x11133aab);
  thread_start (app_th);

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

static void
led_blink ()
{
  static int i;

  if (i ^= 1)
    GPIO_PIN_CLR (1, 18);
  else
    GPIO_PIN_SET (1, 18);
}

continuation_func app0_main __attribute__((noreturn));
void
app0_thread (uint32_t arg)
{

  iprintf ("%s: arg=%x\n", __FUNCTION__, arg);
  adc_init ();
  adc_channel_enable (1);
  while (/*CONSTCOND*/1)
    {
      uint32_t v;
      adc_conversion (1, &v);

      iprintf ("%d\n", v);
      mdelay (100);
    }
  intr_enable ();
  // NOTREACHED
}

void
app0_main ()
{
  led_blink ();
  thread_block (app0_main);
  // NOTREACHED
  while (/*CONSTCOND*/1)
    ;
}

void
board_reset (uint32_t cause __attribute__((unused)))
{

  // Stop timer.
  timer_fini ();

  cpu_reset ();
  while (/*CONSTCOND*/1)
    ;
  // NOTREACHED
}

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | DELAY_CALIBRATE | RAM_CHECK | BUFFERED_CONSOLE_ENABLE;
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{
  // on-board LED
  // GPIO power: always enabled.
  mcu_fast_gpio (TRUE);
  gpio_dir (1, 18, GPIO_OUTPUT); // P1.18 connected to on-board LED.
  *FIO1MASK = 0;
  //LED on
  GPIO_PIN_CLR (1, 18);

  timer_init ();
  vic_init ();
}

uint32_t
test (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{

  return 0;
}
