
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

#define	THREAD_PRIVATE
#include <sys/system.h>
#include <string.h>
#include <sys/console.h>
#include <sys/thread.h>

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

thread_t
md_thread_create (thread_t tc, thread_func_t start, uint32_t arg)
{

  tc->regs.er0 = arg; // pass 1st arg.
  // install return address for 'rte'
  if (tc->tc_stack)
    *(uint32_t *)tc->tc_stack->bottom =
      (addr_t)start | 0x80000000; // disable interrupt.
  else
    tc->continuation = (continuation_func_t)start;

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
  // Already stack is attached.
  tc->regs.sp = (addr_t)tc->tc_stack->bottom;

  if (tc->continuation)
    {
      // [CCR | return address] preserve control register.
      assert (!(tc->regs.ccr & ADDRESS_MASK));
      assert (!((addr_t)tc->continuation & ~ADDRESS_MASK));
      // XXX This should be
      // *(uint32_t *)tc->tc_stack->bottom = 0x80000000 |(addr_t)tc->continuation;
      *(uint32_t *)tc->tc_stack->bottom = tc->regs.ccr |(addr_t)tc->continuation;
      tc->continuation = NULL;	// scheduled now.
    }
}

void
md_thread_continuation_call (continuation_func_t cont)
{
  // Rewind stack and call continuation.
  current_thread->continuation = NULL;	// dispatch here.
  __asm volatile ("mov %0, sp \n jmp @%1"::
		  "r"(current_thread->tc_stack->bottom), "r"(cont));
  // NOTREACHED
}

void
md_thread_continuation_call_with_arg (thread_func_t cont, uint32_t arg)
{
  // Rewind stack and call continuation.
  current_thread->continuation = NULL;	// dispatch here.
#ifdef __NORMAL_MODE__
  __asm volatile ("mov %0, sp \nmov %1, r6\n mov %2, er0\n jmp @er6"::
		  "r"(current_thread->tc_stack->bottom), "r"(cont), "r"(arg));
#else
  __asm volatile ("mov %0, sp \nmov %1, er6\n mov %2, er0\n jmp @er6"::
		  "r"(current_thread->tc_stack->bottom), "r"(cont), "r"(arg));
#endif

  // NOTREACHED
}


#ifdef DEBUG
void
md_thread_debug_reg (struct reg *r)
{
  cpu_status_t s = intr_suspend ();

  iprintf ("er0:%lx\n", r->er0);
  iprintf ("er1:%lx\n", r->er1);
  iprintf ("er2:%lx\n", r->er2);
  iprintf ("er3:%lx\n", r->er3);
  iprintf ("er4:%lx\n", r->er4);
  iprintf ("er5:%lx\n", r->er5);
  iprintf ("er6:%lx\n", r->er6);
  iprintf (" sp:%lx\n", r->sp);
  intr_resume (s);
}
#endif //DEBUG
