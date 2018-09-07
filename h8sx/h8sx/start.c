
/*-
 * Copyright (c) 2010-2011 UCHIYAMA Yasushi.  All rights reserved.
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
#include <sys/console.h>
#include <sys/shell.h>
#include <console_machdep.h>
#include <sys/board.h>	// board dependent initialize hooks.
#include <reg.h>

__BEGIN_DECLS
void machine_startup (void) __attribute__ ((noreturn));
__END_DECLS

#if !defined THREAD_DISABLE && !defined OVERLAY
// Thread local storage for boot context.
STATIC struct thread_control root_tc;
#endif

void
machine_startup ()
{
  // Symbols provided from ldscript.
  extern char stack_start[];	// This is used by interrupt stack after.
  extern char bss_start[], bss_end[];
  extern char data_start[], data_end[];
  extern char VBR[];
#ifndef RAM_PROGRAM
  extern char rom_data_start[], rom_data_end[];
  char *q;
#endif
  static char *p = (char *)1;	//.data section

  // Set VBR
  __asm volatile ("ldc.l %0, vbr" :: "r"(VBR));

  // Clear BSS
  for (p = bss_start; p < bss_end; p++)
    *p = 0;
  // Heap/Stack marker
  for (p = bss_end; p < stack_start; p++)
    *p = 0xff;

#ifndef RAM_PROGRAM
  // Copy DATA section.
  for (p = data_start, q = rom_data_start; p < data_end; p++, q++)
    *p = *q;
#endif
  // XXX These are board specific parameters.
  struct clock_config clk;
  clk.MDCLK = 0;
  clk.input_clock = 12000000;
  clk.ick = CLK_x4;
  clk.pck = CLK_x2;
  clk.bck = CLK_x4;
  // Set Clock.
  clock_init (&clk);

  // Initialize boot console.(Pck 24MHz, 57600bps)
  // See ../1655/boot_console.c for the other configuration sample.
  boot_console_init (TRUE, 12/*BRR*/);

  // Now OK to use printf.
#ifndef POLLING_PROGRAM
  // Initialize delay parameter.
  delay_init ();
#endif
  printf ("stack_start: %#x\n", stack_start);
#ifndef RAM_PROGRAM
  printf ("ROM data: %#x-%#x\n", rom_data_start, rom_data_end);
#endif
  printf ("RAM data: %#x-%#x %dbyte\n", data_start, data_end,
	  data_end - data_start);
  printf ("bss: %#x-%#x %dbyte\n", bss_start, bss_end,
	  bss_end - bss_start);
  uint32_t vbr;
  __asm volatile ("stc.l vbr, %0" : "=r"(vbr));
  printf ("VBR: %#x (%#x)\n", VBR, vbr);
#ifndef POLLING_PROGRAM
  mcu_init ();
  mcu_info ();
#endif
  clock_info ();

  board_device_init (0);
  shell_init ();

#ifndef THREAD_DISABLE
  // Threading support.
  thread_system_init (&root_tc);

#ifndef BUFFERD_CONSOLE_DISABLE
  // Switch to buffered console.
  console_init (TRUE/*boot_flag & BUFFERED_CONSOLE_ENABLE*/);
#endif
#endif

#if !defined THREAD_DISABLE && !defined OVERLAY
  // Run 1st thread with continuation.
  //  md_thread_continuation_call ((continuation_func_t)board_main);
  md_thread_continuation_call_with_arg (board_main, 0x1badface);
#else
  board_main (0);
  iprintf ("overlay module loaded. return to monitor.\n");
#endif

  /* NOTREACHED */
  while (/*CONSTCOND*/1)
    ;
}

void
os_panic (const char *msg)
{

  iprintf ("!!!PANIC!!! %s\n", msg);
  while (/*CONSTCOND*/1)
    ;
  /* NOTREACHED */
}

#include <unistd.h>
void
stack_check (const char *str)
{
  extern char stack_start[];
  extern char bss_end[];
  uint8_t *p, *q;

  q = (uint8_t *)sbrk (0);
  for (p = q; p < (uint8_t *)stack_start; p++)
    if (*p != 0xff)
      break;

  if (str)
    printf ("%s: ", str);

  printf ("heap %#x-%#x (%d), stack %#x-%#x (%d) remain %d\n",
	  bss_end, q, q - (uint8_t *)bss_end,
	  p, stack_start, (uint8_t *)stack_start - p,
	  p - q);
}
