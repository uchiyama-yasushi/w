
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
#include <sys/types.h>
#include <sys/board.h>

#include <sys/console.h>
#include <sys/shell.h>
#include <sys/thread.h>
#include <sys/fiber.h>

#include <x86/gate.h>
#include <x86/cpu.h>
#include <x86/io.h>
#include <dev/pckbc.h>
#include <dev/nmi.h>

#include <stdlib.h>
#include <pcat/interrupt.h>
#include <pcat/device.h>

SHELL_COMMAND_DECL (task);
SHELL_COMMAND_DECL (test);
SHELL_COMMAND_DECL (task3);

struct thread_control app_tc;

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | RAM_CHECK;
}

void
board_device_init (uint32_t arg)
{

  interrupt_init ();
  clock_init ();
  keyboard_init ();
}

void
board_reset (uint32_t arg)
{
  while (/*CONSTCOND*/1)
    pcat_reset ();
}

uint32_t iret_test (void);
void intr_test2 (void);
void ret_test (void);
void userbit_test (void);
void gp_exception_test (void);

int flag;

continuation_func app0_main __attribute__((noreturn));

void
app0_thread (uint32_t arg)
{
  printf ("%s: arg=%x\n", __FUNCTION__, arg);
  flag = 1;

  app0_main ();
}

void
app0_main ()
{
  printf ("APP0\n");
  thread_block (app0_main);
  // NOTREACHED
  while (/*CONSTCOND*/1)
    ;
}

thread_t app_th;

void
board_main (uint32_t arg)
{
  printf ("%s arg=%x\n", __FUNCTION__, arg);
  // Application thread.
  app_th = thread_create_no_stack (&app_tc, "app0", app0_thread,
				   0xabcdef90);
  thread_start (app_th);

  while (!flag)
    {
      thread_rotate_ready_queue ();
    }
  extern void fiber_test_setup (void);
  fiber_test_setup ();

  //  userbit_test ();
  shell_command_register (&task_cmd);
  shell_command_register (&test_cmd);
  shell_command_register (&task3_cmd);

#if 1
  nmi_enable (TRUE);
  intr_enable ();
#endif

  thread_priority (current_thread, PRI_LOW);
  //  while (/*CONSTCOND*/1)
  //    thread_block (NULL);
  shell_prompt (stdin, stdout);
}

#include <x86/tss.h>
#include <x86/callgate.h>
#include "gdt.h"
void
__task0 ()
{
  int i = 0;

  printf ("[%s](%d)\n", __FUNCTION__, i++);
  tss_dump (GDT_TSS_TASK3);
  gate_dump (GDT_GATE_SYSTEMCALL);
  tss_dump_backlink ();
}

uint32_t
task (int32_t argc, const char *argv[])
{

  task0_install (__task0);
  tss_call (GDT_GATE_TASK0);

  return 0;
}

void
__task3 ()
{

  caller_gate (0xaabbcc00, 0xaabbcc01);
}

void
__gate (int arg0, int arg1)
{
  //  uint32_t *p = (uint32_t *)tss_get_current_stack_top0 ();
  //  int i;

  printf ("hello call gate %x %x\n", arg0, arg1);
#if 0
  printf ("stack top = %x\n", (uint32_t)p);
  for (i = 0; i < 16; i++)
    printf ("%x\n", *p--);
#endif
  eflags_dump (eflags_get ());
  tss_dump_current ();
}

uint32_t
task3 (int32_t argc, const char *argv[])
{

  task3_install (__task3);
  callee_gate_install (__gate);

  tss_call (GDT_GATE_TASK3);

  return 0;
}

static int bound[2];
uint32_t
test (int32_t argc, const char *argv[])
{
  union descriptor *d;
  uint32_t r, r0, r1;

  r = r0 = r1 = 0;


  if (argc < 2)
    {
      printf ("Specify exception #.\n");
      return 0;
    }
  switch (atoi(argv[1]))
    {
    case 0:
      // Divided by zero
      __asm volatile ("div %0":: "r"(0));
      break;
    case 1:
      // debug exception. // QEMU bug.
      __asm volatile ("movl %0, %%dr7":: "r"(0x2000));	// GD/DR7
      __asm volatile ("movl %%dr7, %0": "=r"(r));
      eflags_dump (eflags_get ());
      break;
    case 3:
      // break point
      __asm volatile ("int $3");
      break;
    case 4:
      // over flow
      r = eflags_get ();
      r |= (1 << 11);	// set OF
      eflags_set (r);
      __asm volatile ("into");
      break;
    case 5:
      // boundary check
      bound[0] = 3;
      bound[1] = 6;
      for (r0 = 3; r0 < 10; r0++)
	{
	  printf ("%d\n", r0);
	  __asm volatile ("bound %0, bound":: "r"(r0));
	}
      break;
    case 6:
      // Invalid Opcode
      __asm volatile (".long 0xffffffff");
      break;
    case 7:
      // extension_not_available
      // Disable FPU (EM bit of CR0)
      __asm volatile ("movl %%cr0, %0\n orl $4, %0\n movl %0, %%cr0"::"r"(r));
      // ESC (Escape to coprocessor instruction set.
      __asm volatile ("fcos");
      break;
    case 8:
      // double fault. QEMU bug. (don't reflect segment size change.)
      {
	__asm volatile ("movw %0, %%ss":: "r"((uint16_t)GDT_TEST32));
	__asm volatile ("ljmp	$0, $0");
      }
      break;
    case 10:
      // Invalid TSS
      {
	struct tss *tss = tss_get_context (GDT_TSS_TASK0);
	tss_call (GDT_GATE_TASK0);
	tss->cs = GDT_DATA32;	// causes exception.
	tss_call (GDT_GATE_TASK0);
      }
      break;
    case 11:
      // No segment.
      {
	union descriptor *d = descriptor (GDT_TEST32);
	printf ("ok\n");
	__asm volatile ("movw %0, %%gs":: "r"((uint16_t)GDT_TEST32));
	d->desc.access_byte &= ~PRESENT;
	printf ("NG\n");
	__asm volatile ("movw %0, %%gs":: "r"((uint16_t)GDT_TEST32));
      }
      break;
    case 12:
      // Stack fault.
      {
	d = descriptor (GDT_TEST32);
	printf ("ok\n");
	__asm volatile ("movw %0, %%ss":: "r"((uint16_t)GDT_TEST32));
	d->desc.access_byte &= ~PRESENT;
	printf ("NG\n");
	__asm volatile ("movw %0, %%ss":: "r"((uint16_t)GDT_TEST32));
      }
      break;
    case 13:
      // Generic exception
      __asm volatile ("ljmp	$0, $0");
      break;
    case 14:
      // Page fault. XXX notyet
      // CR0 PG bit
      __asm volatile ("movl %%cr0, %0\n orl $0x80000000, %0\n movl %0, %%cr0"::
		      "r"(r));
      break;
    case 16:
      // Floating point error. QEMU bug.
      // CR0 NE bit.
      __asm volatile ("movl %%cr0, %0\n orl $0x20, %0\n movl %0, %%cr0"::"r"(r));
      {
	__asm volatile ("fdiv %st, %st(0)");
	__asm volatile ("wait");
      }
      break;
    case 17:
      // Alignment error. QEMU bug.
      // CR0 AM bit.
      __asm volatile ("movl %%cr0, %0\n orl $0x40000, %0\n movl %0, %%cr0"::"r"(r));
      // Change to CPL3 task.
      tss_call (GDT_GATE_TASK3);

      break;
    case 18:
      // Machine check.
      break;
    }

  return 0;
}

void
userbit_test ()
{
  uint8_t b = 0;

  userbit_get (b);
  printf ("%x\n", b);
  userbit_set ();
  userbit_get (b);
  printf ("%x (1)\n", b);
  userbit_clear ();
  userbit_get (b);
  printf ("%x (0)\n", b);
  userbit_set ();
  userbit_get (b);
  printf ("%x (1)\n", b);
}

void
ret_test ()
{
  uint32_t esp;

  __asm volatile ("movl %%esp, %0" :: "m"(esp));
  printf ("ESP=%x\n", esp);

  printf ("EIP=%x\n", iret_test ());

  __asm volatile ("movl %%esp, %0" :: "m"(esp));
  printf ("ESP=%x\n", esp);
}

void
intr_test2 ()
{
  cpu_status_t s0, s1;

  intr_enable ();
  s0 = intr_suspend ();
  printf ("s0=%x(1)\n", s0);
  s1 = intr_suspend ();
  printf ("s1=%x(0)\n", s1);
  intr_resume (s1);
  printf ("s1=%x s=%x(0)\n", s1, intr_status ());
  intr_resume (s0);
  printf ("s0=%x, s=%x(1)\n", s0, intr_status ());
  intr_disable ();
  printf ("s=%x(0)\n", intr_status ());
}
