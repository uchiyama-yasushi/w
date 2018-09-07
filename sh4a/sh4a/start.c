
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
#include <sys/thread.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <console_machdep.h>
#include <sys/board.h>	// board dependent initialize hooks.
#include <cpu.h>
#include <reg.h>

__BEGIN_DECLS
void machine_startup (void)
#ifndef OVERLAY
 __attribute__ ((noreturn))
#endif
  ;
volatile uint16_t *__sys_flag; // not used now.
__END_DECLS

#if !defined THREAD_DISABLE && !defined OVERLAY
STATIC struct thread_control root_tc;
#endif

void
machine_startup ()
{
  // Symbols provided from ldscript.
  extern char stack_start[];
  extern char bss_start[], bss_end[];
  extern char data_start[], data_end[];
#ifndef RAM_PROGRAM
  extern char rom_data_start[], rom_data_end[];
  char *q;
#endif
  char *p;
#ifdef RAM_CHECK_ENABLE
  bool ram_ok;
#endif
#ifndef OVERLAY
  uint32_t boot_flag;

  // Inquire boot configuration
  boot_flag = board_boot_config ();
#endif
#ifdef RAM_CHECK_ENABLE
  // Check BSS
  if (boot_flag & RAM_CHECK)
    ram_ok = check_ram ((uint32_t)bss_start, (uint32_t)bss_end, FALSE);
  else
    ram_ok = TRUE;
#endif

  // Clear BSS
  for (p = bss_start; p < bss_end; p++)
    *p = 0;
#ifndef RAM_PROGRAM
  // Copy DATA section.
  for (p = data_start, q = rom_data_start; p < data_end; p++, q++)
    *p = *q;
#endif
#ifndef OVERLAY
  // Initialize devices
  boot_console_init (boot_flag & CONSOLE_ENABLE);
#endif
  // Now OK to use printf.

  printf ("stack_start: 0x%A\n", stack_start);
#ifndef RAM_PROGRAM
  printf ("ROM data: 0x%A-0x%A\n", rom_data_start, rom_data_end);
#endif
  printf ("RAM data: 0x%A-0x%A %dbyte\n", data_start, data_end,
	  data_end - data_start);
  printf ("bss: 0x%A-0x%A %dbyte\n", bss_start, bss_end,
	  bss_end - bss_start);
  uint32_t r;
  CPU_GET_SR (r);
  cpu_dump_sr (r);

  // Install default interrupt handler.(place holder)
  interrupt_init ();

#ifdef RAM_CHECK_ENABLE
  printf ("bss memory check ");
  if (ram_ok)
    {
      printf ("passed.\n");
    }
  else
    {
      printf ("***FAILED***\n");
      ram_ok = check_ram ((uint32_t)bss_start, (uint32_t)bss_end, TRUE);
      cpu_sleep ();
    }
#endif
#ifndef OVERLAY
  // Initialize board specific devices.
  board_device_init (boot_flag);
  // Initialize delay parameter.
  delay_init ();
  if (boot_flag & DELAY_CALIBRATE)
    delay_calibrate ();
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
#endif //!OVERLAY

  exception_enable ();

#if !defined THREAD_DISABLE && !defined OVERLAY
  // Run 1st thread with continuation.
  md_thread_continuation_call_with_arg (board_main, 0xac1dcafe);
#else
    board_main (0);
#endif
#ifndef OVERLAY	// Overlay module returns.
  /* NOTREACHED */
  assert (0);
  while (/*CONSTCOND*/1)
    cpu_sleep ();
#endif
}

long
random ()
{
  static int32_t r = 1;

  r = r * 1103515245 + 12345;
  return r & 0x7fffffff;
}
