
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

// Startup routine for ROM program and 'standalone program' of monitor.

#include <sys/system.h>

#include <sys/delay.h>
#ifndef THREAD_DISABLE
#include <sys/thread.h>
#endif
#include <sys/console.h>
#include <sys/shell.h>
#include <console_machdep.h>
#include <sys/board.h>	// board dependent initialize hooks.
#include <reg.h>

__BEGIN_DECLS
void machine_startup (void) __attribute__ ((noreturn));
__END_DECLS

#ifndef THREAD_DISABLE
STATIC struct thread_control root_tc;
#endif

void
machine_startup ()
{
  // Symbols provided from ldscript.
  extern char stack_start[];	// This is used by interrupt stack after.
  extern char bss_start[], bss_end[];
  extern char data_start[], data_end[];
#ifndef RAM_PROGRAM
  extern char rom_data_start[], rom_data_end[];
  char *q;
#endif
  char *p;
  bool ram_ok;
  uint32_t boot_flag;

  // Inquire boot configuration
  boot_flag = board_boot_config ();

  // Check BSS
  if (boot_flag & RAM_CHECK)
    ram_ok = check_ram ((uint32_t)bss_start, (uint32_t)bss_end, FALSE);
  else
    ram_ok = TRUE;

  // Clear BSS
  for (p = bss_start; p < bss_end; p++)
    *p = 0;
#ifndef RAM_PROGRAM
  // Copy DATA section.
  for (p = data_start, q = rom_data_start; p < data_end; p++, q++)
    *p = *q;
#endif

  // Set CPU speed.
  __mcu_mode_save ();	// save booted state.
  mcu_speed (&cpu_clock_conf);

  // Initialize delay parameter.
  delay_init ();

  // Initialize boot console.
  boot_console_init (&uart_clock_conf);
  // Now OK to use printf.

  printf ("stack_start: %p\n", stack_start);
#ifndef RAM_PROGRAM
  printf ("ROM data: %p-%p\n", rom_data_start, rom_data_end);
#endif
  printf ("RAM data: %p-%p %dbyte\n", data_start, data_end,
	  data_end - data_start);
  printf ("bss: %p-%p %dbyte\n", bss_start, bss_end,
	  bss_end - bss_start);
  uint32_t r = 0;
  __asm volatile ("mov %0, sp" : "=r"(r));
  printf ("current stack=%#x\n", r);

  __mcu_mode_print ();	// booted state.
  mcu_mode ();		// current state.

  if (boot_flag & DELAY_CALIBRATE)
    delay_calibrate ();

  if (!ram_ok)
    {
      printf ("bss RAM error.\n");
      ram_ok = check_ram ((uint32_t)bss_start, (uint32_t)bss_end, TRUE);
      cpu_sleep ();
    }
  else
    {
      printf ("bss RAM check passed.\n");
    }

  // Initialize board specific devices.
  board_device_init (boot_flag);

#ifndef THREAD_DISABLE
  // Threading support.
  thread_system_init (&root_tc);

#ifndef BUFFERD_CONSOLE_DISABLE
  // Switch to buffered console.
  console_init (boot_flag & BUFFERED_CONSOLE_ENABLE);
#endif
#endif

#ifndef SHELL_DISABLE
  shell_init ();
#endif

  // Jump to main with changing stack to thread local storage.
#ifndef THREAD_DISABLE
  // Run 1st thread with continuation.
  md_thread_continuation_call_with_arg (board_main, 0x1badface);
#else
  board_main (0);
#endif

  /* NOTREACHED */
  assert (0);
  while (/*CONSTCOND*/1)
    cpu_sleep ();
}
