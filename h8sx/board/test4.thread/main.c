
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
#include <sys/thread.h>

#include <sys/shell.h>
#include <sys/board.h>
#include <sys/delay.h>

#include <sys/srec.h>

#include <1655/timer.h>
#include <frame.h>

#include <stdlib.h>

SHELL_COMMAND_DECL (test);
SHELL_COMMAND_DECL (interrupt);
SHELL_COMMAND_DECL (timer);

void led_init (void);
void led (bool);

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | RAM_CHECK | DELAY_CALIBRATE;
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{

  led_init ();
  timer_init ();
}

void
board_main (uint32_t arg)
{

  SHELL_COMMAND_REGISTER (test);
  SHELL_COMMAND_REGISTER (interrupt);
  SHELL_COMMAND_REGISTER (timer);

  iprintf ("OHAYO %s %s arg=%x\n", __DATE__, __TIME__, arg);

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

void
thread_context_switch_test (const char *arg, uint32_t pc,
			    uint8_t *ef)
{
  printf ("arg=%x PC=%x SP=%x\n", arg, pc, ef);
  frame_dump (ef);
}

void do_thread_switch_test (uint32_t, uint32_t, uint32_t);

void arg_test_callee (uint8_t a, uint8_t b, uint8_t c);

void
arg_test_callee (uint8_t a, uint8_t b, uint8_t c)
{
  printf ("%s: %x %x %x\n", __FUNCTION__, a, b, c);
}


uint32_t
test (int32_t argc __attribute__((unused)),
      const char *argv[] __attribute__((unused)))
{
  int i;

  printf ("%x %lx %ulx\n", 0xac1dcafe, 0xac1dcafe, 0xac1dcafe);


#if 0
  void __delay_calibrate (int);
  for (i = 0; i < argc; i++)
    {
      printf ("%d:%s\n", i, argv[i]);
    }
  if (argc > 1)
    __delay_calibrate (atoi (argv[1]));
  printf ("timer %d\n", *TMR (7, TCNT));
#endif
  for (i = 0;; i++)
    {
      mdelay (1000);
      iprintf ("%d\n", i);
    }

  void arg_test_caller (void);
  arg_test_caller ();

  return 0;
}

uint32_t
interrupt (int32_t argc __attribute__((unused)),
      const char *argv[] __attribute__((unused)))
{
  cpu_status_t s0, s1;

  intr_disable ();
  printf ("%d = 7 disabled\n", intr_status ());
  intr_enable ();
  printf ("%d = 0 enabled\n", intr_status ());
  s0 = intr_suspend ();
  printf ("%d = 0 %d = 7 suspended\n", s0, intr_status ());
  s1 = intr_suspend ();
  printf ("%d = 7 %d = 7 \tsuspended\n", s1, intr_status ());
  s1 = intr_resume (s1);
  printf ("%d = 7 %d = 7 \tresumed\n", s1, intr_status ());
  s0 = intr_resume (s0);
  printf ("%d = 0 %d = 0 resumed\n", s0, intr_status ());

  intr_priority (2);
  printf ("%d = 2 priority\n", intr_status ());

  s0 = intr_priority (4);
  printf ("%d = 2 %d = 4 \tpriority\n", s0, intr_status ());
  s1 = intr_priority (3);
  printf ("%d = 4 %d = 3 \t\tpriority\n", s1, intr_status ());
  s1 = intr_resume (s1);
  printf ("%d = 4 %d = 4 \t\tpriority\n", s1, intr_status ());
  s0 = intr_resume (s0);
  printf ("%d = 2 %d = 2 \tpriority\n", s0, intr_status ());

  intr_disable ();
  printf ("%d = 7 disabled\n", intr_status ());

  return 0;

}

uint32_t
timer (int32_t argc __attribute__((unused)),
       const char *argv[] __attribute__((unused)))
{
  uint8_t os = intr_priority (0);
  iprintf ("old=%d new=%d\n", os, intr_status ());
#if 0
  struct tmr_config tmr;
  tmr.TCORA = 0xff;
  tmr.TCORB = 0;
  tmr.TCCR = TMR_ICKS_div8192_RE;
  tmr.TCR = TMR_CKS_INTCLK_1024_8192 | TCR_CMIEA | TCR_CCLR_CMP_A;
#endif
  printf ("timer %d\n", TMR_GET_CNT (0));

  return 0;
}

void tmr6 (void) __attribute__ ((interrupt_handler));
void tmr5 (void) __attribute__ ((interrupt_handler));
void tmr4 (void) __attribute__ ((interrupt_handler));
void c_tmr0_cmia (struct exception_frame *, uint8_t, uint8_t);
void tmr0_cmib (void) __attribute__ ((interrupt_handler));
void tmr0_ovf (void) __attribute__ ((interrupt_handler));
void tmr1_cmia (void) __attribute__ ((interrupt_handler));
void tmr1_cmib (void) __attribute__ ((interrupt_handler));
void tmr1_ovf (void) __attribute__ ((interrupt_handler));
void tmr2_cmia (void) __attribute__ ((interrupt_handler));
void tmr2_cmib (void) __attribute__ ((interrupt_handler));
void tmr2_ovf (void) __attribute__ ((interrupt_handler));
void tmr3_cmia (void) __attribute__ ((interrupt_handler));
void tmr3_cmib (void) __attribute__ ((interrupt_handler));
void tmr3_ovf (void) __attribute__ ((interrupt_handler));

void
tmr5 ()
{
  static int i;

  *TMR (5, TCSR) &= ~TCSR_CMFA;
  *TMR (5, TCSR); // dummy read.

  iprintf ("ohayo5 %d\n", i++);
}

void
tmr4 ()
{
  static int i;

  *TMR (4, TCSR) &= ~TCSR_CMFA;
  *TMR (4, TCSR); // dummy read.

  iprintf ("ohayo4 %d\n", i++);
}

void
c_tmr0_cmia (struct exception_frame *ef, uint8_t opri, uint8_t pri)
{

  ef->thread_state = 2;

  frame_exception_assert (ef, opri, pri);
  iprintf ("%s\n", __FUNCTION__);

  *TMR (0, TCSR) &= ~TCSR_CMFA;
  *TMR (0, TCSR); // dummy read.
}

void
tmr0_cmib ()
{
  static int i;

  *TMR (0, TCSR) &= ~TCSR_CMFB;
  *TMR (0, TCSR); // dummy read.

  iprintf ("hello0B %d\n", i++);
}

void
tmr0_ovf ()
{
  static int i;

  *TMR (0, TCSR) &= ~TCSR_OVF;
  *TMR (0, TCSR); // dummy read.

  iprintf ("hello0O %d\n", i++);
}

void
tmr1_cmia ()
{
  static int i;

  *TMR (1, TCSR) &= ~TCSR_CMFA;
  *TMR (1, TCSR); // dummy read.

  iprintf ("hello1A %d\n", i++);
}

void
tmr1_cmib ()
{
  static int i;

  *TMR (1, TCSR) &= ~TCSR_CMFB;
  *TMR (1, TCSR); // dummy read.

  iprintf ("hello1B %d\n", i++);
}

void
tmr1_ovf ()
{
  static int i;

  *TMR (1, TCSR) &= ~TCSR_OVF;
  *TMR (1, TCSR); // dummy read.

  iprintf ("hello1O %d\n", i++);
}

void
tmr2_cmia ()
{
  static int i;

  *TMR (2, TCSR) &= ~TCSR_CMFA;
  *TMR (2, TCSR); // dummy read.

  iprintf ("hello2A %d\n", i++);
}

void
tmr2_cmib ()
{
  static int i;

  *TMR (2, TCSR) &= ~TCSR_CMFB;
  *TMR (2, TCSR); // dummy read.

  iprintf ("hello2B %d\n", i++);
}

void
tmr2_ovf ()
{
  static int i;

  *TMR (2, TCSR) &= ~TCSR_OVF;
  *TMR (2, TCSR); // dummy read.

  iprintf ("hello2O %d\n", i++);
}

void
tmr3_cmia ()
{
  static int i;

  *TMR (3, TCSR) &= ~TCSR_CMFA;
  *TMR (3, TCSR); // dummy read.

  iprintf ("hello3A %d\n", i++);
}

void
tmr3_cmib ()
{
  static int i;

  *TMR (3, TCSR) &= ~TCSR_CMFB;
  *TMR (3, TCSR); // dummy read.

  iprintf ("hello3B %d\n", i++);
}

void
tmr3_ovf ()
{
  static int i;

  *TMR (3, TCSR) &= ~TCSR_OVF;
  *TMR (3, TCSR); // dummy read.

  iprintf ("hello3O %d\n", i++);
}
