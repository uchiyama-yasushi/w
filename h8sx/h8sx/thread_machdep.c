
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

#define	THREAD_PRIVATE
#include <sys/system.h>
#include <string.h>
#include <sys/console.h>
#include <sys/thread.h>
#include <frame.h>

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

STATIC
void
__thread_stack_frame_setup (thread_t tc, thread_func_t start, uint32_t arg)
{
  // Allocate the 1st frame.
  struct exception_frame *ef = (struct exception_frame *)
    (tc->tc_stack->bottom - sizeof (struct exception_frame));
  struct exception_stack_frame *esf = &ef->stack_frame;

  memset (ef, 0, sizeof *ef);

  // Link it.
  tc->regs.ef = ef;

  // Setup stack frame.
#if 0
  esf->exr = 0x0;	// Interrupt enabled on enter.
#else
  esf->exr = 0x7;	// Interrupt disabled on enter.
#endif
  esf->ccr_pc = (addr_t)start; // install return address for 'rte'

  // 1st argument.
  ef->er0 = arg;

  // This is exception frame.
  ef->flags = 1;
  ef->thread_state = THREAD_STATE_START;	// for debug
}

thread_t
md_thread_create (thread_t tc, thread_func_t start, uint32_t arg)
{

  if (tc->tc_stack)
    {
      // The 1st frame.
      __thread_stack_frame_setup (tc, start, arg);
    }
  else
    {
      // Deffered util stack will be allocated.
      tc->continuation_arg = arg;
      tc->continuation = (continuation_func_t)start;
    }

  return tc;
}

void
md_thread_stack_discard (thread_t tc)
{
  // H8SX port don't have dedicated exception frame.
  tc->regs.ef = NULL;
}

void
md_thread_continuation_setup (thread_t tc)
{
  // Setup register set for the next context switch.
  // Already stack is attached.
  __thread_stack_frame_setup (tc, (thread_func_t)tc->continuation,
			      tc->continuation_arg);

  if (tc->continuation)
    {
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
  __asm volatile ("mov %0, sp \n"
		  "mov %1, er6\n"
		  "mov %2, er0\n"
		  "jmp @er6"::
		  "r"(current_thread->tc_stack->bottom), "r"(cont), "r"(arg));

  // NOTREACHED
}


#ifdef DEBUG
void
md_thread_debug_reg (struct reg *r)
{
  cpu_status_t s = intr_suspend ();
  uint8_t *frame = (uint8_t *)r->ef;

  struct switch_frame *sf;
  struct exception_frame *ef;
  struct exception_stack_frame *esf;
  bool is_exception_frame = frame[0] & 0x1;

  iprintf ("%x\n", frame[0]);
  if (is_exception_frame)
    {
      ef = (struct exception_frame *)frame;
      esf = &ef->stack_frame;
      iprintf ("EXP\ner0:%x\ner1:%x\ner2:%x\ner3:%x\n"
	       "er4:%x\ner5:%x\ner6:%x\n\n",
	       ef->er0, ef->er1, ef->er2, ef->er3,
	       ef->er4, ef->er5, ef->er6);
    }
  else
    {
      sf = (struct switch_frame *)frame;
      esf = &sf->stack_frame;
      iprintf ("SW\ner4:%x\ner5:%x\ner6:%x\n\n", sf->er4, sf->er5, sf->er6);
    }
  uint32_t ccr_pc = esf->ccr_pc;

  iprintf ("exr=%x\nccr=%x\npc=%x\n", esf->exr, ccr_pc >> 24, ccr_pc & 0xffffff);
  intr_resume (s);
}

void
__thread_switch_assert (uint8_t *frame)
{
  uint8_t opri;
  bool is_exception_frame = frame[0] & 0x1;

  if (is_exception_frame)
    {	// Exception frame.
      struct exception_frame *ef = (struct exception_frame *)frame;
      opri = ef->stack_frame.exr & 0x7;
    }
  else
    {	// Switch frame.
      struct switch_frame *sf = (struct switch_frame *)frame;
      opri = sf->stack_frame.exr & 0x7;
    }

  if (opri != 0 &&
      (frame[1] != THREAD_STATE_START)/* On enter, interrupt disabled */)
    {
      iprintf ("XXX INVALID CONTEXT SWITCH opri=%d (%s frame)\n", opri,
	       is_exception_frame ? "exception" : "switch");
      iprintf ("current_thread=%s\n", current_thread->name);
      frame_dump (frame);
    }
}
#endif //DEBUG
