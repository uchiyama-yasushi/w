
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
#include <sys/console.h>
#include <x86/cpu.h>
#include <x86/tss.h>
#include <x86/gate.h>
#include <x86/exception.h>
#include "gdt.h"
#include <string.h> //memset

#define	TSS_DEFAULT_STACK_SIZE	4096
#define	TSS_DEFAULT_LDT_NUM	4
#define	TSS_DEFAULT_LDT_SIZE	(4 * sizeof (union descriptor))

struct task_local_storage
{
  struct tss tss;
  union descriptor ldt[TSS_DEFAULT_LDT_NUM];
  uint8_t stack[TSS_DEFAULT_STACK_SIZE];
  uint8_t stack_privilege[TSS_DEFAULT_STACK_SIZE];
} __attribute__ ((aligned (4096))); // struct tss limitation.

struct task_local_storage task[5];
STATIC struct tss bootstrap_tss;

STATIC void task_user0 (void);
STATIC void task_user3 (void);
STATIC void (*user_task0)(void);
STATIC void (*user_task3)(void);

#define	EXCEPTION_TASK(n, cause, func)					\
    {									\
      &task[n].tss,							\
      GDT_TSS_ ## cause, GDT_GATE_ ## cause,				\
	0,	/* priority 0 */					\
      GDT_CODE32, GDT_DATA32,						\
      { task[n].ldt, TSS_DEFAULT_LDT_SIZE, 0, GDT_LDT ## n },		\
      { task[n].stack, TSS_DEFAULT_STACK_SIZE, 0, GDT_DATA32 },		\
      { 0, 0, 0, 0 },/* No privilege change. */				\
      0,								\
      func, #cause							\
    }

#define	APPLICATION_TASK(n, app, func, priority)			\
    {									\
      &task[n].tss,							\
      GDT_TSS_ ## app, GDT_GATE_ ## app,				\
      priority,								\
      GDT_CODE32, GDT_DATA32,						\
      { task[n].ldt, TSS_DEFAULT_LDT_SIZE, priority, GDT_LDT ## n },	\
      { task[n].stack, TSS_DEFAULT_STACK_SIZE, priority, GDT_DATA32 },	\
      { task[n].stack_privilege, TSS_DEFAULT_STACK_SIZE, 0, GDT_DATA32 },\
      0,								\
      func, #app							\
    }

struct tss_config tss_config [] =
  {
    EXCEPTION_TASK (0, DOUBLE_FAULT, task_exception_entry),
    EXCEPTION_TASK (1, INVALID_TSS, task_exception_entry),
    EXCEPTION_TASK (2, STACK_OVERRUN, task_exception_entry),
    APPLICATION_TASK (3, TASK0, task_user0, 0),
    APPLICATION_TASK (4, TASK3, task_user3, 3),
  };

void
task_init ()
{
  struct tss_config *config;
  int i;

  // Setup my backup area.
  tss_install_gdt (&bootstrap_tss, GDT_TSS_KERNEL, GDT_GATE_KERNEL);
  strncpy (bootstrap_tss.osdep, "kernel", TSS_OSDEP_MAX);

  // Load myself. (no task change here.)
  tss_set_tr (GDT_TSS_KERNEL);
  printf ("%s: tss:0x%x size:0x%x\n", __FUNCTION__, &bootstrap_tss,
	  sizeof bootstrap_tss);

  // Setup TSS
  for (i = 0, config = tss_config; i < sizeof tss_config / sizeof tss_config[0];
       i++, config++)
    {
      memset (config->stack.start, 0xa5, config->stack.size);
      if (config->stack_privilege.start)
	memset (config->stack_privilege.start, 0xa5,
		config->stack_privilege.size);

      tss_setup (config);
      tss_install_gdt (config->tss, config->tss_segment, config->gate_selector);
    }
}

void
task0_install (void (*func)(void))
{

  user_task0 = func;
}

void
task3_install (void (*func)(void))
{

  user_task3 = func;
}

void
task_user0 ()
{

  while (/*CONSTCOND*/1)
    {
      if (user_task0)
	user_task0 ();
      __asm volatile ("iret");
    }
}

void
task_user3 ()
{

  while (/*CONSTCOND*/1)
    {
      if (user_task3)
	user_task3 ();
      __asm volatile ("iret");
    }
}
