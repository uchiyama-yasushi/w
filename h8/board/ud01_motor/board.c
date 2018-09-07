
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
#include <sys/console.h>
#include <sys/delay.h>
#include <sys/shell.h>

#include <reg.h>
#include <sys/board.h>

#include <stdlib.h>
#include <string.h>
//#include "mmc.h"
#include <sysvbfs/bfs.h>
#include <pcat_mbr.h>
#include <sys/disklabel_netbsd.h>

SHELL_COMMAND_DECL (test);


void
board_main (uint32_t arg __attribute__((unused)))
{
  shell_command_register (&test_cmd);

  *PA_DDR = 0xff; // All output.
  *PA_DR = 0; // All low.

  printf ("-----start------\n");
}


uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | BUFFERED_CONSOLE_ENABLE | DELAY_CALIBRATE;
}


uint32_t
test (int32_t argc __attribute__((unused)),
	   const char *argv[] __attribute__((unused)))
{
  static int i;

  i ^= 1;
  printf ("hello! %d\n", i);
  *PA_DR = i;

  return 0;
}
