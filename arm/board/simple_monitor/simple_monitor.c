
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

#include <sys/srec.h>
#include <reg.h>
#include <console_machdep.h>

SHELL_COMMAND_DECL (l);

// Clock configuration.
struct cpu_clock_conf cpu_clock_conf =
  {
    //SYSCLK 288MHz, CCLK 72MHz.
  m:12,
  n:1,
  cpudiv:4,
  oscillator_freq: 12000000,
  };

struct uart_clock_conf uart_clock_conf =
 { 0, 3, 5 | (8 << 4), CCLK4 }; // PCLK 18.0000MHz 230769bps error 0.16%
//  { 0, 6, 5 | (8 << 4), CCLK4 }; // PCLK 18.0000MHz 115385bps error 0.16%

void fix_vector_checksum (addr_t);

void
board_main (uint32_t arg __attribute__((unused)))
{

  shell_command_register (&l_cmd);

  iprintf ("LPC2388 Simple Monitor Build %s %s\n", __DATE__, __TIME__);

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

void
board_reset (uint32_t cause __attribute__((unused)))
{

  cpu_reset ();
  // NOTREACHED
  while (/*CONSTCOND*/1)
    ;
}

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | RAM_CHECK | DELAY_CALIBRATE;
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{
  // on-board LED
  // GPIO power: always enabled.
  mcu_fast_gpio (TRUE);
  *FIO1MASK = 0;
  //LED off
  GPIO_PIN_SET (1, 18);

  // Check User program area.
  if (!check_ram (0x40000000, 0x4000f800, TRUE))
    {
      printf ("*** memory error ***\n");
      cpu_sleep ();
    }
}

uint32_t
l (int32_t argc __attribute__((unused)),
   const char *argv[] __attribute__((unused)))
{
  cpu_status_t s;
  int32_t sz;
  srec_return_t ret;
  uint32_t err = -1;

  iprintf ("Send S-record file.\n");

  s = intr_suspend ();
  srec_init ();
  ret = srec_load (&sz);

  iprintf ("Read %D byte. %s\n", sz, srec_error_message[ret]);
  if (ret != SREC_EOF)
    {
      iprintf ("Failed.\n");
      goto failed;
    }
  iprintf ("Start address: 0x%x\n", srec_start_addr);

  // Change vector table location.
  *MEMMAP = MAP_USERRAM;

  // jump to user program.
  __asm volatile ("mov pc, %0;nop;nop" :: "r" (srec_start_addr));
  iprintf ("done.\n");
  err = 0;
  // Return to Rom-monitor's vector table.
  *MEMMAP = MAP_USERFLASH;

 failed:
  intr_resume (s);

  return err;
}

void
fix_vector_checksum (addr_t vector_addr)
{
  int32_t *vector = (int32_t *)vector_addr;
  int i;
  int checksum = 0;
  for (i = 0; i < 8; i++)
    {
      if (i == 5)
	continue;
      checksum += vector[i];
    }
  vector[5] = -checksum;
}
