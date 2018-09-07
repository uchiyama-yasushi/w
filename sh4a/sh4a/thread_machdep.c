
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

#define	THREAD_PRIVATE
#include <sys/system.h>
#include <string.h>
#include <sys/console.h>
#include <sys/thread.h>
#include <cpu.h>

#ifdef THREAD_DEBUG
#define	DPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif
#ifdef THREAD_VERBOSE
#define	LPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	LPRINTF(fmt, args...)	((void)0)
#endif

void
md_thread_noreturn_assert ()
{
  printf ("*** noreturn function return from 0x%x. ***\n",
	  __builtin_return_address (0));
  while (/*CONSTCOND*/1)
    ;
  // NOTREACHED
}

thread_t
md_thread_create (thread_t tc, thread_func_t start, uint32_t arg)
{
  // Setup register set for the 1st context switch.
  tc->regs.pr = (addr_t)md_thread_noreturn_assert;
  tc->regs.pc = (addr_t)start;
  tc->regs.r4 = arg;
  // Privilege-mode, Register Bank 1, Exception Enable, Interrupt Disable,
  // FPU Enable. (XXX?)
  tc->regs.sr = SR_MD | SR_RB | (SR_IMASK_MASK << SR_IMASK_SHIFT);

  return tc;
}

void
md_thread_stack_discard (thread_t tc)
{

  tc->regs.sp = 0;
}

void
md_thread_continuation_setup (thread_t tc)
{
  // Setup register set for the next context switch.
  tc->regs.sp = (addr_t)tc->tc_stack->bottom;
  tc->regs.pr = (addr_t)md_thread_noreturn_assert;
  if (tc->continuation)
    tc->regs.pc = (addr_t)tc->continuation;
  tc->continuation = NULL;	// scheduled now.
}

extern void __md_thread_jump (uint32_t, thread_func_t, uint8_t *);
void
md_thread_continuation_call (continuation_func_t cont)
{
  // Rewind stack and call continuation.
  current_thread->continuation = NULL;	// dispatch here.
  __asm volatile ("mov %0, r15;jmp @%1;nop"::
		  "r"(current_thread->tc_stack->bottom), "r"(cont));
  // NOTREACHED
}

void
md_thread_continuation_call_with_arg (thread_func_t cont, uint32_t arg)
{
  // Rewind stack and call continuation.
  current_thread->continuation = NULL;	// dispatch here.
  __md_thread_jump (arg, cont, current_thread->tc_stack->bottom);
  // NOTREACHED
}


#ifdef DEBUG
void
md_thread_debug_reg (struct reg *regs)
{
  cpu_status_t s = intr_suspend ();
  uint32_t *r = (uint32_t *)regs;
  uint32_t i;

  iprintf ("addr = %x\n", (addr_t)regs);
  for (i = 0; i < sizeof *regs / sizeof (uint32_t); i++)
    {
      iprintf ("%d: %x\n", i, r[i]);
    }
  intr_resume (s);
}
#endif //DEBUG
