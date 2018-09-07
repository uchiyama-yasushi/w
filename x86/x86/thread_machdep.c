
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
#include <x86/cpu.h>

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

void thread_x86_iret_setup (struct iret_frame *, addr_t, uint32_t);

void
md_thread_noreturn_assert ()
{
  printf ("*** noreturn function return from 0x%x. ***\n",
	  __builtin_return_address (0));
  while (/*CONSTCOND*/1)
    ;
}

// construct IRET frame when stack is newly allocated.
void
thread_x86_iret_setup (struct iret_frame *iret, addr_t func, uint32_t arg)
{

  iret->eip = func;
  iret->arg = arg;
  iret->cs = cs_get ();
  iret->eflags = eflags_get () & ~0x200;// Disable interrupt.
  iret->noreturn = (addr_t)md_thread_noreturn_assert;
}

thread_t
md_thread_create (thread_t tc, thread_func_t start, uint32_t arg)
{
  /*
    Interrupt handler.
    Stack usage with no privilege-level change.

    +---------------+
    |     EFLAGS    |-4
    +---------------+
    |     CS        |-8
    +---------------+
    |     EIP       |-12 <-ESP
    +---------------+
    |  Error code   |-16 <-ESP (8,10,11,12,13,14,17)
    +---------------+
    |               |
           ...
    |               | stack_top
    +---+---+---+---+
      3   2   1   0


    THIS ROUTINES SETTING

                     stack_bottom
    +---------------+
    |  1st arg      |-4
    +---------------+
    |return address |-8
    +---------------+
    |     EFLAGS    |-12
    +---------------+
    |     CS        |-16
    +---------------+
    |     EIP       |-20  <-ESP
    +---------------+
    |               |
           ...
    |               | stack_top
    +---+---+---+---+
      3   2   1   0
   */
  // install return address for 'IRET'
  if (tc->tc_stack)
    {
      tc->regs.sp = (addr_t)tc->tc_stack->bottom - sizeof (struct iret_frame);
      // Setup stack for 'IRET'
      thread_x86_iret_setup ((struct iret_frame *)tc->regs.sp,
			     (addr_t)start, arg);
    }
  else
    {
      // IRET frame is constructed when stack is allocated.
      //XXX Now struct thread has continuation_arg member.
      //XXX Use it instead of this hack.
      tc->regs.sp = arg;	//XXX kludge hack.
      tc->continuation = (continuation_func_t)start;
    }
  LPRINTF ("[%d]:%s func=%A esp=%x\n",  tc->id, tc->name, start, tc->regs.sp);

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
  uint32_t arg = tc->regs.sp;	//XXX kludge hack.
  // Setup register set for the next context switch.
  // Already stack is attached.
  tc->regs.sp = (addr_t)tc->tc_stack->bottom - sizeof (struct iret_frame);
  if (tc->continuation)
    {
      // Setup stack for 'IRET'
      thread_x86_iret_setup ((struct iret_frame *)tc->regs.sp,
			     (addr_t)tc->continuation, arg);
      tc->continuation = NULL;	// scheduled now.
    }
}

void
md_thread_continuation_call (continuation_func_t cont)
{
  // Rewind stack and call continuation.
  current_thread->continuation = NULL;	// dispatch here.
  __asm volatile ("movl %1, %%esp\n"
		  "pushl %0\n"
		  "jmp *%2" ::
		  "g"(md_thread_noreturn_assert),
		  "g"(current_thread->tc_stack->bottom), "r"(cont));
  // NOTREACHED
}

void
md_thread_continuation_call_with_arg (thread_func_t cont, uint32_t arg)
{
   // Rewind stack and call continuation with arg.
  current_thread->continuation = NULL;	// dispatch here.
  __asm volatile ("movl %2, %%esp\n"
		  "pushl %0\n"
		  "pushl %1\n"
		  "jmp *%3" ::
		  "r"(arg),
		  "g"(md_thread_noreturn_assert),
		  "g"(current_thread->tc_stack->bottom), "r"(cont));
}

#ifdef DEBUG
void
md_thread_debug_reg (struct reg *r)
{
  cpu_status_t s = intr_suspend ();

  iprintf ("eax:%x\n", r->eax);
  iprintf ("ebx:%x\n", r->ebx);
  iprintf ("ecx:%x\n", r->ecx);
  iprintf ("edx:%x\n", r->edx);
  iprintf ("esi:%x\n", r->esi);
  iprintf ("edi:%x\n", r->edi);
  iprintf ("ebp:%x\n", r->ebp);
  iprintf ("esp:%x\n", r->sp);
  intr_resume (s);
}
#endif //DEBUG
