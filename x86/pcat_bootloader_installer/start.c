
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
#include <sys/delay.h>
#include <sys/thread.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <sys/board.h>

#include <x86/segment.h>
#include <x86/gate.h>
#include <x86/cpu.h>
#include "gdt.h"

#include <pcat/bios.h>
#include <dev/pckbc.h>
#include <dev/nmi.h>

__BEGIN_DECLS
void machine_startup (int) __attribute__ ((noreturn, regparm(1)));
void installer_init (int);
__END_DECLS

#ifndef THREAD_DISABLE
STATIC struct thread_control root_tc;
#endif

// Already A20 is enabled.
void __attribute__ ((noreturn, regparm(1)))
machine_startup (int boot_drive)
{
  // Symbols provided from ldscript.
  extern char _stack_start[];
  extern char bss_start[], bss_end[];
  extern char data_start[], data_end[];
  char *p;

  // Load GDT
  extern struct lgdt_arg lgdt_arg;
  __asm volatile ("lgdt %0" :: "m"(lgdt_arg));

  // Save previous IDT
  struct lidt_arg oldidt;
  __asm volatile ("sidt %0" : "=m"(oldidt):);

  // Clear BSS
  for (p = bss_start; p < bss_end; p++)
    *p = 0;

  // Initialize BIOS console.
#ifdef SERIAL_CONSOLE
  bios_console_init (SERIAL);
#else
  bios_console_init (NORMAL);
#endif
  // Now OK to use printf.
  printf ("BBM INSTALLER build %s %s\n", __DATE__, __TIME__);
  printf ("stack_start: 0x%A\n", _stack_start);
  printf ("data: 0x%A-0x%A %dbyte\n", data_start, data_end,
	  data_end - data_start);
  printf ("bss: 0x%A-0x%A %dbyte\n", bss_start, bss_end,
	  bss_end - bss_start);
  printf ("boot drive: 0x%x\n", boot_drive);

#ifndef THREAD_DISABLE
  // Threading support.
  thread_system_init (&root_tc);
#endif

  shell_init ();

  // Jump to main with changing stack to thread local storage.
#ifndef THREAD_DISABLE
  md_thread_continuation_call_with_arg (board_main, boot_drive);
#else
    board_main (0);
#endif

  /* NOTREACHED */
  assert (0);
  while (/*CONSTCOND*/1)
    cpu_sleep ();
}

void
board_reset (uint32_t arg)
{
  while (/*CONSTCOND*/1)
    pcat_reset ();
}

void
board_main (uint32_t arg)
{

  installer_init (arg);
  nmi_enable (FALSE);

  shell_prompt (stdin, stdout);
}

