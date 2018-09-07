
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

uint8_t app0_tls[THREAD_STACK_SIZE (THREAD_STACKSIZE_DEFAULT)]
__attribute ((aligned (4)));
void app0_thread (uint32_t);


SHELL_COMMAND_DECL (led);
SHELL_COMMAND_DECL (sleep);
SHELL_COMMAND_DECL (swi);
SHELL_COMMAND_DECL (suspend);
SHELL_COMMAND_DECL (resume);
SHELL_COMMAND_DECL (intr);
SHELL_COMMAND_DECL (jump);
SHELL_COMMAND_DECL (delay);

thread_t app_th;
thread_t root_th;

void fiber_test_setup (void);

void
board_main (uint32_t arg __attribute__((unused)))
{
  root_th = current_thread;

  // Undefined instruction test.
  //  __asm volatile (".long 0xffffffff"); // don't exception.
  //  __asm volatile ("mrc	p15, #0, r0, c0, c0, #0");
  // Data abort test
  //__asm volatile ("ldr	r0, [%0]" :: "r"(0x50000000));
  // Prefetch abort test.
  //__asm volatile ("mov	pc, %0":: "r" (0x50000000));
  fiber_test_setup ();
  shell_command_register (&led_cmd);
  shell_command_register (&sleep_cmd);
  shell_command_register (&swi_cmd);
  shell_command_register (&suspend_cmd);
  shell_command_register (&resume_cmd);
  shell_command_register (&intr_cmd);
  shell_command_register (&jump_cmd);
  shell_command_register (&delay_cmd);


  // Application thread.
  app_th = thread_create_with_stack (app0_tls, THREAD_STACKSIZE_DEFAULT, "app0",
			  app0_thread, 0x11133aab);
  thread_start (app_th);

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

void
app0_thread (uint32_t arg)
{
  int k;
  iprintf ("%s: arg=%x\n", __FUNCTION__, arg);

  while (/*CONSTCOND*/1)
    {
      for (k = 0; k < 100; k++)
	if (!timer_sleep (1000))
	  iprintf ("couldn't sleep\n");
      led (0, 0);
    }
  // NOTREACHED
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

  vic_init ();
  timer_init ();
}

uint32_t
delay (int32_t argc __attribute__((unused)),
       const char *argv[] __attribute__((unused)))
{
  int i;

  for (i = 0; i < 60; i++)
    {
      led (0, 0);
      mdelay (200);
    }

  return 0;
}

uint32_t
led (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
  static int i;

  if (i ^= 1)
    GPIO_PIN_CLR (1, 18);
  else
    GPIO_PIN_SET (1, 18);

  return 0;
}

int __intr_stat[8];
int __nest;
uint32_t
suspend (int32_t argc __attribute__((unused)),
	 const char *argv[] __attribute__((unused)))
{
  __intr_stat[__nest++] = intr_suspend ();
  return 0;
}

uint32_t
resume (int32_t argc __attribute__((unused)),
	const char *argv[] __attribute__((unused)))
{
  intr_resume (__intr_stat[--__nest]);
  return 0;
}


uint32_t
swi (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{

  __asm volatile ("swi #334");

  iprintf ("return from swi %s %x\n", __FUNCTION__);

  return 0;
}

void
ohayo_func ()
{
  iprintf ("ohayo\n");
  while (1)
    ;
}

uint32_t
sleep (int32_t argc __attribute__((unused)),
       const char *argv[] __attribute__((unused)))
{

  cpu_psr_dump (cpu_psr_get ());
  thread_block (NULL);
  iprintf ("%s: wakeup!\n", __FUNCTION__);

  return 0;
}

uint32_t
jump (int32_t argc __attribute__((unused)),
      const char *argv[] __attribute__((unused)))
{
  uint32_t r[16];
  int i;

  memset (r, 0, sizeof r);
  iprintf ("%x\n", r);
  cpu_regs_save (r);
  for (i = 0; i < 16; i++)
    {
      iprintf ("%x\n", r[i]);
    }
  r[15] = (uint32_t)ohayo_func;
  cpu_regs_load (r);
  iprintf ("OHAYO\n");
  return 0;
}


uint32_t
intr (int32_t argc __attribute__((unused)),
      const char *argv[] __attribute__((unused)))
{
  uint32_t r;
  cpu_status_t s, s0, s1;

  __asm volatile ("mrs %0, cpsr": "=r"(r): );

  cpu_psr_dump (cpu_psr_get ());

  intr_enable ();
  printf ("enabled\n");
  cpu_psr_dump (cpu_psr_get ());

  intr_disable ();
  printf ("diabled\n");
  cpu_psr_dump (cpu_psr_get ());

  s = intr_suspend ();
  printf ("suspended %x\n", s);
  cpu_psr_dump (cpu_psr_get ());

  intr_resume (s);
  printf ("resumed %x\n", s);
  cpu_psr_dump (cpu_psr_get ());

  intr_enable ();
  printf ("enabled\n");
  cpu_psr_dump (cpu_psr_get ());

  s = intr_suspend ();
  printf ("suspended %x\n", s);
  cpu_psr_dump (cpu_psr_get ());

  intr_resume (s);
  printf ("resumed %x\n", s);
  cpu_psr_dump (cpu_psr_get ());

  intr_enable ();
  s0 = intr_suspend ();
  printf ("s0=%x(0)\n", s0);
  // now intrrupt disabled.
  s1 = intr_suspend ();
  // now intrrupt disabled.
  printf ("s1=%x(c0)\n", s1);
  intr_resume (s1);
  // now intrrupt disabled.
  printf ("s1=%x s=%x(c0)\n", s1, intr_status ());
  intr_resume (s0);
  // now intrrupt enabled.
  printf ("s0=%x, s=%x(0)\n", s0, intr_status ());
  intr_disable ();
  // now intrrupt disabled.
  printf ("s=%x(c0)\n", intr_status ());
  return 0;
}
