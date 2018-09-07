
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

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>

#include <sys/shell.h>
#include <sys/board.h>

#include <sys/srec.h>

SHELL_COMMAND_DECL (l);

void led_init (void);
void led (bool);

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE;
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{

  led_init ();
}

void
board_main (uint32_t arg __attribute__((unused)))
{

  SHELL_COMMAND_REGISTER (l);

  iprintf ("H8SX/1655 Simple Monitor Build %s %s\n", __DATE__, __TIME__);

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

void
led_init ()
{

  BSET (3, PM_DDR);
}

void
led (bool on)
{
  if (on)
    BCLR (3, PM_DR);
  else
    BSET (3, PM_DR);
}

uint32_t
l (int32_t argc __attribute__((unused)),
   const char *argv[] __attribute__((unused)))
{
  int32_t sz;
  srec_return_t ret;
  uint32_t err = -1;

  srec_init ();
  ret = srec_load (&sz);

  iprintf ("Read %d byte. %s\n", sz, srec_error_message[ret]);
  if (ret != SREC_EOF)
    {
      iprintf ("Failed.\n");
      goto failed;
    }
  iprintf ("Start address: 0x%x\n", srec_start_addr);

  // jump to user program.
  __asm volatile ("jsr @%0" :: "r" (srec_start_addr));
  iprintf ("done.\n");
  err = 0;

  // Return to Rom-monitor's vector table.
  __asm volatile ("ldc.l %0, vbr" :: "r"(0));

 failed:

  return err;
}
