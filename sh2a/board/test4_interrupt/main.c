
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

#include <sys/shell.h>
#include <sys/board.h>
#include <reg.h>
#include <sys/debug.h>

#include <stdlib.h>

#include <console_machdep.h> //XXX
#include <sys/thread.h>

SHELL_COMMAND_DECL (test);
SHELL_COMMAND_DECL (e);
SHELL_COMMAND_DECL (interrupt);

void irq3_init (void);

void
board_main (uint32_t arg __attribute__((unused)))
{
#define	PCIOR0	((volatile uint16_t *)0xfffe3852)
#define	PCCR2	((volatile uint16_t *)0xfffe384a)
#define	PCDR	((volatile uint16_t *)0xfffe3856)

  *PCCR2 &= ~0x3;	// PC8
  *PCIOR0 |= (1 << 8);	// PC8 output
  *PCDR |= (1 << 8);	// LED off.

  SHELL_COMMAND_REGISTER (test);
  SHELL_COMMAND_REGISTER (e);
  SHELL_COMMAND_REGISTER (interrupt);

  __asm volatile ("ldc %0, gbr" :: "r"(0xac1dcafe));
  irq3_init ();

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

void
irq3_init ()
{

  *PGCR2 &= ~(7 << 12); // PG11 mode
  *PGCR2 |= (3 << 12);	// PG11 is IRQ3

  intc_irq_sense (3, ICR1_FALLING_EDGE);
  intc_priority (INTPRI_IRQ3, INTPRI_13);
}

void
c_irq3 (uint32_t sp)
{

  extern void frame_dump (uint8_t *);

  iprintf ("IRQ3! sp=%x priority=%d\n", sp, intr_status () >> 4);
#if 0
  static int nbank;
  if (++nbank > 15)
    {
      frame_dump ((uint8_t *)sp);
    }
  intr_enable ();
  __asm volatile ("movi20 %0, r6" :: "i"(0xdead));
  while (/*CONSTCOND*/1)
    ;
#else
  frame_dump ((uint8_t *)sp);
#endif
}


uint32_t
test (int32_t argc __attribute__((unused)),
   const char *argv[] __attribute__((unused)))
{
  static int blink = 0;

  if (blink)
    *PCDR &= ~(1 << 8);	// LED on.
  else
    *PCDR |= (1 << 8);	// LED off.
  blink ^= 1;

  intc_priority (INTPRI_IRQ4, INTPRI_8);

  printf ("%x\n", *(uint32_t *)0x1c000018);
  uint32_t r = 0;
  __asm volatile ("movi20 #-116662, %0" : "=r"(r));
  printf ("movi20 #-116662=%x\n", r);	//0xfffe384a
  __asm volatile ("movi20s #-29865472, %0" : "=r"(r));
  printf ("movi20s #-29865472=%x\n", r);	//0xfe384a00
  __asm volatile ("movi20 #0xf0, %0" : "=r"(r));
  printf ("movi20 #0xf0=%x\n", r);	// 0xf0
  __asm volatile ("movi20s #0xf00, %0" : "=r"(r));
  printf ("movi20s #0xf00=%x\n", r);	// 0xf00
  __asm volatile ("movi20 #-241, %0" : "=r"(r));
  printf ("movi20 #-241=%x\n", r); //0xffffff0f
  uint8_t b = 0;

  BSET (2, &b);
  printf ("bset 2 %x\n", b);
  BSET (0, &b);
  printf ("bset 0 %x\n", b);
  BCLR (2, &b);
  printf ("bclr 2 %x\n", b);
  BCLR (0, &b);
  printf ("bclr 0 %x\n", b);
  uint16_t hw = 0xdead;
  printf ("%x %x\n", SWAP16 (hw), hw);
  uint32_t w = 0xac1dcafe;
  printf ("%x %x\n", SWAP32 (w), w);
  printf ("128->%d 127->%d -127->%d -128->%d -150->%d\n",
	  CLIPS_B(128), CLIPS_B(127), CLIPS_B(-127), CLIPS_B(-128),
	  CLIPS_B(-150));

  printf ("%x %x %x\n", CLIPU_W (0xac1dcafe), CLIPU_W (0xfff1),
	  CLIPU_B (-127));

  return 0;
}

uint32_t
interrupt (int32_t argc __attribute__((unused)),
      const char *argv[] __attribute__((unused)))
{
  cpu_status_t s0, s1;

  bitdisp32 (intr_status ());
  intr_enable ();
  printf ("enable");
  bitdisp32 (intr_status ());
  intr_disable ();
  printf ("diable");
  bitdisp32 (intr_status ());
  intr_enable ();
  printf ("enable");
  bitdisp32 (intr_status ());
  printf ("suspend");
  bitdisp32 (s0 = intr_suspend ());
  bitdisp32 (intr_status ());
  printf ("suspend");
  bitdisp32 (s1 = intr_suspend ());
  bitdisp32 (intr_status ());
  printf ("resume");
  intr_resume (s1);
  bitdisp32 (intr_status ());
  printf ("resume a");
  intr_resume (s0);
  bitdisp32 (intr_status ());
  printf ("pri3");
  bitdisp32 (intr_priority (3 << 4));
  bitdisp32 (intr_status ());
  bitdisp32 (cpu_sr_get ());
  printf ("pri7");
  bitdisp32 (intr_priority (7 << 4));
  bitdisp32 (intr_status ());
  bitdisp32 (cpu_sr_get ());
  intr_resume (s0);
  printf ("resume a");
  bitdisp32 (intr_status ());

  intr_disable ();
  printf ("%d = 15 disabled\n", intr_status () >> 4);
  intr_enable ();
  printf ("%d = 0 enabled\n", intr_status () >> 4);
  s0 = intr_suspend ();
  printf ("%d = 0 %d = 15 suspended\n", s0 >> 4, intr_status () >> 4);
  s1 = intr_suspend ();
  printf ("%d = 15 %d = 15 \tsuspended\n", s1 >> 4, intr_status () >> 4);
  s1 = intr_resume (s1);
  printf ("%d = 15 %d = 15 \tresumed\n", s1 >> 4, intr_status () >> 4);
  s0 = intr_resume (s0);
  printf ("%d = 0 %d = 0 resumed\n", s0 >> 4, intr_status () >> 4);

  intr_priority (2 << 4);
  printf ("%d = 2 priority\n", intr_status () >> 4);

  s0 = intr_priority (4 << 4);
  printf ("%d = 2 %d = 4 \tpriority\n", s0 >> 4, intr_status () >> 4);
  s1 = intr_priority (3 << 4);
  printf ("%d = 4 %d = 3 \t\tpriority\n", s1 >> 4, intr_status () >> 4);
  s1 = intr_resume (s1);
  printf ("%d = 4 %d = 4 \t\tpriority\n", s1 >> 4, intr_status () >> 4);
  s0 = intr_resume (s0);
  printf ("%d = 2 %d = 2 \tpriority\n", s0 >> 4, intr_status () >> 4);

  intr_disable ();
  printf ("%d = 15 disabled\n", intr_status () >> 4);

  return 0;
}

uint32_t
e (int32_t argc, const char *argv[])
{
  int i = -1;

  if (argc > 1)
    i = atoi (argv[1]);
  printf ("test=%d\n", i);
  switch (i)
    {
    default:
      break;
    case 0:
      __asm volatile (".align 2;.long 0xffffffff");
      break;
    case 1:
      __asm volatile ("resbank");
      break;

    }

  return 0;
}
