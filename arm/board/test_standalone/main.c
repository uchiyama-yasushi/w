
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

// Clock configuration.
struct cpu_clock_conf cpu_clock_conf =
  {
    //SYSCLK 288MHz, CCLK 72MHz.
  m:12,
  n:1,
  cpudiv:4,
  oscillator_freq: 12000000,
  };

#include <console_machdep.h>
struct uart_clock_conf uart_clock_conf =
 { 0, 3, 5 | (8 << 4), CCLK4 }; // PCLK 18.0000MHz 230769bps error 0.16%
//  { 0, 6, 5 | (8 << 4), CCLK4 }; // PCLK 18.0000MHz 115385bps error 0.16%

SHELL_COMMAND_DECL (led);
SHELL_COMMAND_DECL (test);

void
board_main (uint32_t arg __attribute__((unused)))
{

  shell_command_register (&led_cmd);
  shell_command_register (&test_cmd);

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

void
board_reset (uint32_t cause __attribute__((unused)))
{

  cpu_reset ();
  while (/*CONSTCOND*/1)
    ;
  // NOTREACHED
}

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE;
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

void
ohayo_func ()
{
  iprintf ("ohayo\n");
  while (1)
    ;
}

uint32_t
test (int32_t argc __attribute__((unused)),
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

#if 0
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
#endif
  return 0;
}
