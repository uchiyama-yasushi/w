
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
#include <sys/delay.h>
#include <sys/thread.h>
#include <sys/console.h>
#include <sys/shell.h>

#include <x86/segment.h>
#include <x86/gate.h>
#include <x86/cpu.h>

#include <pcat/a20.h>
#include <pcat/bios.h>
#include <sys/board.h>

#include <sysvbfs/bfs.h>
#include <dev/pckbc.h> // pcat_reset
#include "bootloader.h"

STATIC struct block_io_ops bios_io =
  {
    NULL,
    bios_disk_drive,
    bios_disk_read,
    bios_disk_read_n,
    bios_disk_write,
    bios_disk_write_n
  };
STATIC uint8_t bios_hdd_context[sizeof (struct bios_hdd) + DEV_BSIZE];

void
board_reset (uint32_t arg)
{

  while (/*CONSTCOND*/1)
    pcat_reset ();
}

__BEGIN_DECLS
void machine_startup (void) __attribute__ ((noreturn));
__END_DECLS
static bool a20_ok __attribute__((section (".data")));
void
machine_startup ()
{
  // Symbols provided from ldscript.
  extern char _stack_start[];
  extern char bss_start[], bss_end[];
  extern char data_start[], data_end[];
  char *p;

  delay_init ();	// A20 gate needs delay.
  // Gate A20 on
  a20_ok = a20_enable (TRUE);

  // Clear BSS
  for (p = bss_start; p < bss_end; p++)
    *p = 0;

  // Initialize BIOS console.
  bios_console_init (NORMAL);
  // Now OK to use printf.
  printf ("BBM build %s %s\n", __DATE__, __TIME__);
  printf ("stack: 0x%A-0x%A %dbyte\n", _stack_start, bss_end,
	  _stack_start - bss_end);
  printf ("data: 0x%A-0x%A %dbyte\n", data_start, data_end,
	  data_end - data_start);
  printf ("bss: 0x%A-0x%A %dbyte\n", bss_start, bss_end,
	  bss_end - bss_start);
  printf ("A20: %sabled.\n", a20_ok ? "en" : "dis");

  //  delay_calibrate ();
  shell_init ();

  bios_io.cookie = bios_disk_init (bios_hdd_context, boot_drive);
  bootloader_init (&bios_io);

  shell_prompt (stdin, stdout);

  /* NOTREACHED */
  assert (0);
  while (/*CONSTCOND*/1)
    cpu_sleep ();
}
