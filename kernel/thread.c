
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
#include <sys/thread_stack.h>

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

STATIC thread_id_t thread_id_max;
SIMPLEQ_HEAD (tc_queue, thread_control);

struct thread_control *current_thread;	// global.

STATIC struct tc_queue  thread_ready_queue[THREAD_PRIORITY_MAX];
struct thread_list thread_list;	// export to thread_machdep.c

// No thread is activated, last thread context is saved to this.
STATIC struct reg saved_regs;
STATIC thread_t saved_thread;

static inline struct tc_queue *
__ready_queue (struct thread_control *tc)
{
  return thread_ready_queue + tc->priority;
}
__BEGIN_DECLS
STATIC void thread_setup (thread_t, const char *);
STATIC thread_t thread_select (void);
int thread_context_switch (continuation_func_t);
int do_thread_switch (continuation_func_t);
__END_DECLS
#ifdef DEBUG
bool thread_find_ready_queue (thread_id_t); // for debug.
void md_thread_debug_stack_usage (void);
#endif
STATIC thread_t thread_find_by_id (thread_id_t);

void
thread_system_init (thread_t tc)
{
  int i;
  cpu_status_t s = intr_suspend ();

  // Prepare stack pool.
  thread_stack_init ();
  assert (thread_stack_loaded);
  // Ready queue.
  for (i = 0; i < THREAD_PRIORITY_MAX; i++)
    SIMPLEQ_INIT (thread_ready_queue + i);
  // Thread list.
  SLIST_INIT (&thread_list);

  // Initialize myself.
  thread_setup (tc, "root");

  // Allocate stack.
  thread_stack_attach (tc, thread_stack_allocate (), NULL);

  current_thread = tc;
  current_thread->state = THR_RUN;
  SIMPLEQ_INSERT_HEAD (__ready_queue (current_thread), current_thread, tc_link);
  intr_resume (s);
}

thread_t
thread_create_with_stack (uint8_t *thread_area, size_t stack_size,
			  const char *name, thread_func_t start, uint32_t arg)
{
  cpu_status_t s = intr_suspend ();
  struct thread_control *tc = (struct thread_control *)thread_area;

  thread_setup (tc, name);
  assert (stack_size);
  // Setup dedicated stack area.
  thread_stack_dedicate (thread_area, stack_size);
#ifdef __H8300SX__
#else
  tc->regs.sp =(addr_t)tc->tc_stack->bottom;
#endif
  stack_size = tc->tc_stack->size;

  // Setup 1st continuation.
  md_thread_create (tc, start, arg);

  LPRINTF ("[%d]:%s pc=%p sp=%p stack=%ldbyte\n",
	   tc->id, tc->name, start, (void *)tc->tc_stack->bottom, stack_size);
  intr_resume (s);

  return tc;
}

thread_t
thread_create_no_stack (thread_t tc, const char *name, thread_func_t start,
			uint32_t arg)
{
  cpu_status_t s = intr_suspend ();

  thread_setup (tc, name);
  // Stack will be allocated when thread is scheduled.
  md_thread_stack_discard (tc);
  md_thread_create (tc, start, arg);

  LPRINTF ("[%d]:%s (no stack)\n", tc->id, tc->name);
  intr_resume (s);

  return tc;
}

void
thread_setup (thread_t tc, const char *name)
{
  memset (tc, 0, sizeof (struct thread_control));

  tc->id = ++thread_id_max;
  tc->state = THR_DORMANT;
  tc->priority = PRI_APPHI;
  tc->name = name;

  SLIST_INSERT_HEAD (&thread_list, tc, t_link);
}

int
thread_start (thread_t tc)
{
  cpu_status_t s = intr_suspend ();
  enum error_code err = EPERM;

  if (tc->state == THR_DORMANT)
    {
      tc->state = THR_READY;
      SIMPLEQ_INSERT_TAIL (__ready_queue (tc), tc, tc_link);
      err = 0;
    }
  intr_resume (s);
  LPRINTF ("[%d]\n", tc->id);
  assert (!err);

  return err;
}

int
thread_block (continuation_func_t cont)
{
  cpu_status_t s = intr_suspend ();
  thread_t old_thread;
  bool block;

  old_thread = current_thread;
  block = --old_thread->wakeup_request < 0;
#ifdef THREAD_VERBOSE
  if (old_thread->state != THR_RUN)
    {
      LPRINTF ("blocking not running thread [%s]\n", old_thread->name);
    }
#endif
  assert (old_thread->state == THR_RUN);
  if (block)
    {
      // Remove old_thread from run queue.
      // Continuation is saved at thread_context_switch() <- do_thread_switch().
      assert (SIMPLEQ_FIRST (__ready_queue (old_thread)) == old_thread);
      SIMPLEQ_REMOVE_HEAD (__ready_queue (old_thread), tc_link);
      old_thread->state = THR_WAIT;
    }
  else
    {
      // If no need to block, directly call continuation.
      intr_resume (s);
      if (cont)
	{
	  md_thread_continuation_call (cont);
	  // NOTREACHED
	  assert (0);
	}
      return 0;
    }
  do_thread_switch (cont);

  intr_resume (s);

  return 0;
}

int
thread_wakeup_once (thread_t tc)
{
  cpu_status_t s = intr_suspend ();

  if (tc->wakeup_request < 0)
    {
      thread_wakeup (tc);
    }
  intr_resume (s);

  return 0;
}

int
thread_wakeup (thread_t tc)
{
  cpu_status_t s = intr_suspend ();
  int err = 0;

  assert (tc->wakeup_request < 127); // Wakeup over flow

  switch (tc->state)
    {
    case THR_RUN:	// request from interrupt handler.
      /* FALLTHROUGH */
    case THR_READY:
      assert (thread_find_ready_queue (tc->id));
      tc->wakeup_request++;
      break;
    case THR_WAIT:
      tc->state = THR_READY;
      tc->wakeup_request++;
      assert (!thread_find_ready_queue (tc->id));
      SIMPLEQ_INSERT_TAIL (__ready_queue (tc), tc, tc_link);
      break;
    case THR_DORMANT:
      /*FALLTHROUGH*/
    case THR_NONEXISTENT:
      iprintf ("nonexsitent/dormant thread.\n");
      err = EPERM;
      break;
    }
  intr_resume (s);

  return err;
}

int
thread_context_switch (continuation_func_t cont)
{
  thread_t old_thread, new_thread;

  DPRINTF ("===============================================\n");
#ifdef THREAD_DEBUG
  if (cont)
    {
      DPRINTF ("CONTINUATION=>%x\n", cont);
    }
#endif

  old_thread = current_thread;
  if (cont)
    old_thread->continuation = cont;

  if ((new_thread = thread_select ()) == 0)
    return 0;	// No thread is available. spin again.

  // Runnable thread is selected. update thread status.
  if (old_thread != new_thread && old_thread->state == THR_RUN)
    {
      DPRINTF ("%d preempted by %d\n", old_thread->id, new_thread->id);
      old_thread->state = THR_READY;
    }

  // It is possible that new_thread has no stack. currently use
  // old_thread's stack
  // If new_thread has continuation, that thread don't have dedicated stack.
  if (old_thread->continuation) // old_thread has discardable stack,
    {
      // handoff stack to new_thread
      if (new_thread->continuation)
	{
	  DPRINTF ("STACK HANDOFF (%s:%x->%s:%x)\n", old_thread->name,
		   old_thread->continuation,
		   new_thread->name, new_thread->continuation);
	  thread_stack_handoff (old_thread, new_thread);
	  md_thread_continuation_setup (new_thread);
	}
      else
	{
#if 0
	  thread_debug_state (0);
	  iprintf ("%x %x %x\n", old_thread, new_thread, current_thread);
	  iprintf ("DISCARD STACK (%s)\n", old_thread->name);
#endif
	  DPRINTF ("DISCARD STACK (%s)\n", old_thread->name);
	  // discard it for the other thread.
	  thread_stack_detach (old_thread);
	}
    }

  // If new_thread has no stack, newly allocate. thraed is resumed
  // from continuation in such a thread.
  if (!new_thread->tc_stack)
    {
      DPRINTF ("ALLOCATE STACK (%d:%s)\n", new_thread->id, new_thread->name);
      thread_stack_attach (new_thread, thread_stack_allocate (),
			   new_thread->continuation);
      if (new_thread->continuation)
	{
	  md_thread_continuation_setup (new_thread);
	}
    }

  // Now change thread.
  current_thread = new_thread;
  current_thread->state = THR_RUN;

#ifdef DEBUG
  thread_debug_state_check ();
#endif

  //  md_thread_debug_reg (&current_thread->regs);
  //  DPRINTF ("switch %d->%d: sp=%x pc=%x\n", old_thread->id, new_thread->id,
  //	   new_thread->regs.sp, new_thread->regs.pc);
#ifdef __H8300SX__
  DPRINTF ("switch %d->%d: sp=%x\n", old_thread->id, new_thread->id,
	   new_thread->regs.ef);
#else
  DPRINTF ("switch %d->%d: sp=%x\n", old_thread->id, new_thread->id,
	   new_thread->regs.sp);
#endif
  return 0;
}

// Find thread next to run. if not found, spin in this routine.
thread_t
thread_select ()
{
  // already interrupt disabled.
  struct thread_control *tc;
  struct tc_queue *q;
  int i;

#ifdef __H8300SX__
  DPRINTF ("{%x}: sp=%x\n", current_thread, current_thread->regs.ef);
#else
  DPRINTF ("{%x}: sp=%x\n", current_thread, current_thread->regs.sp);
#endif
  //  md_thread_debug_reg (&current_thread->regs);

  // Find next thread.
  for (i = 0, tc = NULL; i < THREAD_PRIORITY_MAX; i++)
    {
      q = thread_ready_queue + i;
      if ((tc = SIMPLEQ_FIRST (q)) != NULL)
	break;
    }

  // If saved thread is exists, restore it.
  if (tc && saved_thread)
    {
      DPRINTF ("\t\t\t\t\t%s: spin thread %d restored.\n", __FUNCTION__,
	       saved_thread->id);
      memcpy (&saved_thread->regs, &saved_regs, sizeof (struct reg));
      saved_thread = NULL;
    }

  // No thread in ready queue.
  if (!tc)
    {
      if (saved_thread)
	{
	  assert (saved_thread == current_thread);
	  DPRINTF ("return to spin\n");
	  return 0;	// Spin again. (will jump to (*))
	}
      saved_thread = current_thread;
      memcpy (&saved_regs, &current_thread->regs, sizeof (struct reg));

      DPRINTF ("\t\t\t\t\t%s: spin %d\n", __FUNCTION__, current_thread->id);
      intr_enable ();
      while (/*CONSTCOND*/1)
	;	// Waiting for interrupt. (*)
      assert (0);
      /*NOTREACHED*/
    }

  if (current_thread != tc && current_thread->state == THR_RUN)
    {
      DPRINTF ("%d preempted by %d\n", current_thread->id, tc->id);
      current_thread->state = THR_READY;
    }

  return tc;
}

int
thread_priority (thread_t tc, thread_priority_t npri)
{
  cpu_status_t s = intr_suspend ();
  struct tc_queue *new_q, *old_q;
  int opri;
  int err = 0;

  assert (npri >= 0 && npri < THREAD_PRIORITY_MAX);
  opri = tc->priority;
  tc->priority = npri;

  new_q = thread_ready_queue + npri;
  old_q = thread_ready_queue + opri;

  DPRINTF ("[%d] pri %d->%d state = %d (%s)\n", tc->id, opri, npri, tc->state,
	   npri > opri ? "DOWN" : "UP");
  switch (tc->state)
    {
    case THR_RUN:
      SIMPLEQ_REMOVE_HEAD (old_q, tc_link);
      SIMPLEQ_INSERT_TAIL (new_q, tc, tc_link);
      if (npri >= opri) // new priority is eqaual or lower.
	{
	  tc->state = THR_READY;
	  do_thread_switch (NULL);	// no continuation.
	}
      break;

    case THR_READY:
      SIMPLEQ_REMOVE (old_q, tc, thread_control, tc_link);
      SIMPLEQ_INSERT_TAIL (new_q, tc, tc_link);
      if (current_thread->priority > tc->priority)
	{
	  do_thread_switch (NULL);	// no continuation.
	}
      break;
    case THR_WAIT:
      // nothing to do.
      break;
    case THR_DORMANT:
      /*FALLTHROUGH*/
    case THR_NONEXISTENT:
      DPRINTF ("nonexsitent/dormant thread.\n");
      err = EPERM;
      break;
    }
  intr_resume (s);

  return err;
}

void
thread_priority_inherit_enter (thread_t tc)
{

  DPRINTF ("%s: inherit priority(set) [%d]%d->[%d]%d.\n",
	   __FUNCTION__,
	   current_thread->id, current_thread->priority,
	   tc->id, tc->priority);
  if (tc->inherit_priority) // don't overwrite native priority.
    return;

  tc->native_priority = tc->priority; // save native priority.
  tc->inherit_priority = TRUE;

  thread_priority (tc, THREAD_INHERIT_PRIORITY);
}

void
thread_priority_inherit_exit (thread_t tc)
{

  if (!tc->inherit_priority)
    return;

  DPRINTF ("%s: inherit priority(clear) [%d] (%d->%d)\n",
	   __FUNCTION__, tc->id, tc->priority, tc->native_priority);
  tc->inherit_priority = FALSE;
  thread_priority (tc, tc->native_priority);
}

int
thread_destroy (thread_id_t id)
{
  cpu_status_t s = intr_suspend ();
  thread_t tc = thread_find_by_id (id);
  thread_t t;
  struct tc_queue *q;
  int err = 0;

  if (!tc || tc->state == THR_RUN)
    {
      err = EPERM;
      goto exit;
    }

  // Remove from thread list.
  SLIST_REMOVE (&thread_list, tc, thread_control, t_link);

  if (tc->state == THR_READY)
    {
      q = __ready_queue (tc);
      SIMPLEQ_FOREACH (t, q, tc_link)
	{
	  if (t == tc)
	    {
	      SIMPLEQ_REMOVE (q, tc, thread_control, tc_link);
	      goto deleted;
	    }
	}
      assert (0);	// bogus thread state.
    }
 deleted:
  tc->state = THR_DORMANT;

 exit:
  intr_resume (s);
  return err;
}

thread_t
thread_find_by_id (thread_id_t id)
{
  // must be called with interrupt disabled.
  struct thread_control *tc;

  SLIST_FOREACH (tc, &thread_list, t_link)
    {
      if (tc->id == id)
	return tc;
    }

  return NULL;
}

#ifdef DEBUG
bool
thread_find_ready_queue (thread_id_t id)
{
  struct thread_control *tc;
  struct tc_queue *q;
  int i;

  for (i = 0, tc = NULL; i < THREAD_PRIORITY_MAX; i++)
    {
      q = thread_ready_queue + i;
      SIMPLEQ_FOREACH (tc, q, tc_link)
	{
	  if (tc->id == id)
	    return TRUE;
	}
    }
  return FALSE;
}

void
thread_debug_state (uint32_t arg __attribute__((unused)))
{
  cpu_status_t s = intr_suspend ();
  char state[] = "RrWDN";
  struct thread_control *tc;
  struct tc_queue *q;
  int i;

  iprintf ("---Ready Queue---");
  for (i = 0, tc = NULL; i < THREAD_PRIORITY_MAX; i++)
    {
      iprintf ("\n<%d>: ", i);
      q = thread_ready_queue + i;
      SIMPLEQ_FOREACH (tc, q, tc_link)
	{
	  iprintf ("%d ", tc->id);
	}
    }
  iprintf ("\n");
  iprintf ("---Thread Status---\nid   pri(used/total)\n");
  thread_debug_stack_check ();
  SLIST_FOREACH (tc, &thread_list, t_link)
    {
      size_t sz = 0;
      iprintf ("[%d] %c %d ", tc->id, state[tc->state], tc->priority);
      if (tc->tc_stack)
	{
	  sz = tc->tc_stack->bottom - tc->tc_stack->top;
	  iprintf ("(%ld/%ld) ",  sz - tc->tc_stack->remain, sz);
	}
      else
	{
	  iprintf ("(no stack) ");
	}
      iprintf ("%s\n", tc->name);
    }
  iprintf ("---Stack Status---\n  bottom   top     (used/total)\n");
  thread_stack_debug_state ();

  intr_resume (s);
}

void
thread_debug_stack_check ()
{
  cpu_status_t s = intr_suspend ();
  struct thread_control *tc;

  SLIST_FOREACH (tc, &thread_list, t_link)
    {
      struct thread_stack *ts = tc->tc_stack;
      if (!ts)	// thread that diacardable stack may not have it.
	{
	  ts->remain = 0;
	  continue;
	}
      if (!thread_stack_debug_count_remain (ts))
	{
	  iprintf ("thread %d stack overflow. %lx\n", tc->id,
		   *(uint32_t *)ts->top);
	}
    }
  intr_resume (s);
}

void
thread_debug_state_check ()
{
  struct thread_control *tc;
  struct tc_queue *q;
  int i, j;

  for (i = 0, j = 0, tc = NULL; i < THREAD_PRIORITY_MAX; i++)
    {
      q = thread_ready_queue + i;
      SIMPLEQ_FOREACH (tc, q, tc_link)
	{
	  if (tc->state == THR_RUN)
	    j++;
	}
    }
  assert (j <= 1);
}
#endif //DEBUG
