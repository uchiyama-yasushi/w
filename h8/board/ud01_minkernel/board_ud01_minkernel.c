
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

#include <reg.h>

#include <sys/board.h>

#include <libud01/ud01dev.h>


uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | RAM_CHECK | DELAY_CALIBRATE;
}

void
board_main (uint32_t arg __attribute__((unused)))
{
  extern char bss_start[], bss_end[];
#ifdef RAM_PROGRAM
  extern void board_bsc_init (void);

  board_bsc_init ();
#endif

  iprintf ("[BSS] ");
  if (check_ram ((uint32_t)bss_start, (uint32_t)bss_end, TRUE))
    iprintf ("OK\n");
  else
    iprintf ("NG\n");

#if 0
  volatile int *p = (volatile int *)ADDR1M_AREA4_START;
  int i;
  for (i = 0; i < 16; i++)
    {
      *p = (1 << i);
      iprintf ("%x %x\n", (int)*p, (int)(1 << i));
    }
#endif
  if (1)
    {
      // Check RAM exclude BSS. (Development board only)
      iprintf ("[AREA 4] ");
      if (check_ram (ADDR1M_AREA4_START, ADDR1M_AREA5_START - 1, TRUE))
	iprintf ("OK\n");
      else
	iprintf ("NG\n");

      iprintf ("[AREA 5] ");
      if (check_ram (ADDR1M_AREA5_START, ADDR1M_AREA6_START - 1, TRUE))
	iprintf ("OK\n");
      else
	iprintf ("NG\n");
    }

  while (/*CONSTCOND*/1)
    cpu_sleep ();
}

