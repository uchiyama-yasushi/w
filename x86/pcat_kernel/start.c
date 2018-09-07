
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
#include <x86/tss.h>
#include <x86/cpu.h>
#include <x86/exception.h>
#include <x86/callgate.h>
#include "gdt.h"

#include <pcat/bios.h>	//XXX

#include <sys/board.h>	// board dependent initialize hooks.

__BEGIN_DECLS
void machine_startup (void) __attribute__ ((noreturn));
__END_DECLS

#ifndef THREAD_DISABLE
STATIC struct thread_control root_tc;
#endif

// x86 generic startup.
void
machine_startup ()
{
  // Symbols provided from ldscript.
  extern char _stack_start[];
  extern char bss_start[], bss_end[];
  extern char data_start[], data_end[];
  char *p;
  uint32_t boot_flag;

  // Load GDT
  extern struct lgdt_arg lgdt_arg;
  __asm volatile ("lgdt %0" :: "m"(lgdt_arg));

  // Save previous IDT
  struct lidt_arg oldidt;
  __asm volatile ("sidt %0" : "=m"(oldidt):);

  // Inquire boot configuration
  boot_flag = board_boot_config ();

  // Clear BSS
  for (p = bss_start; p < bss_end; p++)
    *p = 0;

  // Initialize BIOS console.
#ifdef SERIAL_CONSOLE
  bios_console_init (SERIAL);	//XXX
#else
  void vga_console_init (void);
  bios_console_init (NORMAL);	//XXX
  vga_console_init ();	//XXX
#endif
  // Now OK to use printf.
  printf ("stack_start: 0x%A\n", _stack_start);
  printf ("data: 0x%A-0x%A %dbyte\n", data_start, data_end,
	  data_end - data_start);
  printf ("bss: 0x%A-0x%A %dbyte\n", bss_start, bss_end,
	  bss_end - bss_start);
  printf ("EFLAGS=0x%x\n", eflags_get ());
  printf ("IDT: base=0x%x limit=0x%x\n", oldidt.base, oldidt.limit);

  // Initialize TSS, IDT, GDT(gate)
  exception_init ();
  task_init ();
  callgate_init ();

  // Initialize delay parameter.
#if 0	//XXX notyet.
  delay_init ();
  delay_calibrate ();
#endif
  // Initialize board specific devices.
  board_device_init (boot_flag);

#ifndef THREAD_DISABLE
  // Threading support.
  thread_system_init (&root_tc);
#ifndef BUFFERD_CONSOLE_DISABLE
  // Switch to buffered console.
  //  sci_console_init (boot_flag & BUFFERED_CONSOLE_ENABLE);
#endif
#endif

#ifndef SHELL_DISABLE
  shell_init ();
#endif

  // Jump to main with changing stack to thread local storage.
#ifndef THREAD_DISABLE
  md_thread_continuation_call_with_arg (board_main, 0xdeadbeaf);
#else
  board_main (0);
#endif

  /* NOTREACHED */
  assert (0);
  while (/*CONSTCOND*/1)
    cpu_sleep ();
}
