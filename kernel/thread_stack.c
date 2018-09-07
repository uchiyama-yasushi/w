
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

// Thread stack allocator.
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

STATIC uint8_t stack_pool_area[THREAD_STACK_POOL_MAX][THREAD_STACKSIZE_DEFAULT]
__attribute ((aligned (16)));
STATIC struct thread_stack stack_pool[THREAD_STACK_POOL_MAX];

// All detachable stack pool.
struct thread_stack_pool thread_stack_pool;
int thread_stack_loaded;

void
thread_stack_init ()
{
  // Prepare stack pool.

  SIMPLEQ_INIT (&thread_stack_pool);
  int i;
  for (i = 0; i < THREAD_STACK_POOL_MAX; i++)
    {
      thread_stack_setup (stack_pool + i, stack_pool_area[i],
			  sizeof (stack_pool_area[0]));
      thread_stack_load (stack_pool + i);
    }
}

void
thread_stack_setup (struct thread_stack *ts, uint8_t *top, size_t size)
{
  ts->top = top;
  ts->size = size;

  // Fill with magic number to estimate stack usage.
  memset (ts->top, THREAD_STACK_MAGIC, size);

#if BYTE_ORDER == BIG_ENDIAN
  ts->bottom = ts->top + ts->size - 4;
  *(uint32_t *)(ts->top) = (uint32_t)0xac1dcafe; // canary
#else
  ts->bottom = ts->top + ts->size;
  *(uint32_t *)ts->top = (uint32_t)0xac1dcafe; // canary
#endif
  LPRINTF ("%p-%p (%lx)\n", ts->bottom, ts->top, ts->size);
}

void
thread_stack_load (struct thread_stack *ts)
{
  cpu_status_t s = intr_suspend ();

  thread_stack_loaded++;
  ts->active = FALSE;
  SIMPLEQ_INSERT_HEAD (&thread_stack_pool, ts, ts_link);

  intr_resume (s);
}

void
thread_stack_dedicate (uint8_t *thread_area,
		       size_t sz /* don't include thread_control area*/)
{
  uint8_t *p = thread_area;
  struct thread_control *tc = (struct thread_control *)p;
  p += sizeof (struct thread_control);
  struct thread_stack *ts = (struct thread_stack *)p;
  p += sizeof (struct thread_stack);

  thread_stack_setup (ts, p, sz - sizeof (struct thread_stack));
  tc->tc_stack = ts;
}

struct thread_stack *
thread_stack_allocate ()
{
  struct thread_stack *ts = NULL;
  cpu_status_t s = intr_suspend ();

  ts = SIMPLEQ_FIRST (&thread_stack_pool);
  assert (ts);
  assert (!ts->active);
  SIMPLEQ_REMOVE_HEAD (&thread_stack_pool, ts_link);
  ts->active = TRUE;

  DPRINTF ("%x\n", ts->bottom);
  intr_resume (s);

  return ts;
}

void
thread_stack_deallocate (struct thread_stack *ts)
{
  cpu_status_t s = intr_suspend ();

  ts->active = FALSE;
  SIMPLEQ_INSERT_HEAD (&thread_stack_pool, ts, ts_link);
  DPRINTF ("%x\n", ts->bottom);
  intr_resume (s);
}

void
thread_stack_handoff (thread_t old_thread, thread_t new_thread)
{
  // We assume current_thread can detach stack.
  new_thread->tc_stack = old_thread->tc_stack;
  if (new_thread != old_thread)
    old_thread->tc_stack = NULL;
}

void
thread_stack_attach (thread_t tc, struct thread_stack *ts,
		     continuation_func_t cont)
{
  tc->tc_stack = ts;
  tc->continuation = cont;
  md_thread_continuation_setup (tc);

  DPRINTF ("[%d] %x-%x\n", tc->id, tc->tc_stack->bottom, tc->tc_stack->top);
}

void
thread_stack_detach (thread_t tc)
{
  // Return stack to pool
  DPRINTF ("[%d] %x-%x\n", tc->id, tc->tc_stack->bottom, tc->tc_stack->top);
  thread_stack_deallocate (tc->tc_stack);
  md_thread_stack_discard (tc);
  tc->tc_stack = NULL;
}

#ifdef DEBUG
void
thread_stack_debug_state ()
{
  struct thread_stack *ts;
  bool overflow;
  int i;

  for (i = 0, ts = stack_pool; i < THREAD_STACK_POOL_MAX; i++, ts++)
    {
      overflow = !thread_stack_debug_count_remain (ts);
      iprintf ("%d %p-%p (%ld/%ld) %s\n", i, ts->bottom, ts->top,
	       ts->size - ts->remain, ts->size,
	       overflow ? "!OVERFLOW!" : "");
    }
}

bool
thread_stack_debug_count_remain (struct thread_stack *ts)
{
  uint8_t *p;
  int i;

  for (i = 0, p = ts->top + 4;	// skip canary
       *p == THREAD_STACK_MAGIC && p != ts->bottom; i++, p++)
    ;
  ts->remain = i + 4;	// add canary space.

  // Check stack overflow.
  return *(uint32_t *)ts->top == 0xac1dcafe; // canary alive?
}
#endif
