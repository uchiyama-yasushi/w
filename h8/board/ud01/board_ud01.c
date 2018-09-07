
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

SHELL_COMMAND_DECL (ledtest);

__BEGIN_DECLS
void push_switch_init (void);
void reg_test (void);
__END_DECLS

void
board_main (uint32_t arg)
{
  iprintf ("hello world arg=%lx\n", arg);
  //  push_switch_init ();
  shell_command_register (&ledtest_cmd);

#ifndef OVERLAY
  // Make shell of this thread.
  shell_prompt (stdin, stdout);
  /* NOTREACHED */
#endif
}

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | BUFFERED_CONSOLE_ENABLE | DELAY_CALIBRATE;
}

uint32_t
ledtest (int32_t argc, const char *argv[])
{
  int select;
  bool on;
  //  uint8_t ccr;
  //  ccr = 39;

  printf ("%ld\n", random ());
  //  assert (0);
  //  bitdisp8 (ccr);
  int i;
  for (i = 0; i < 10; i++)
    {
      led (0, 1);
      mdelay (100);
      led (0, 0);
      mdelay (100);
    }

  if (argc < 3)
    return -1;
  select = atoi (argv[1]);
  on = atoi (argv[2]);

  led (select, on);

  return 0;
}
