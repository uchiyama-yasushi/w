
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

#ifndef _THREAD_H_
#define	_THREAD_H_
#include <machine/thread.h>
#include <sys/queue.h>

#ifndef THREAD_PRIORITY_MAX
#define	THREAD_PRIORITY_MAX		4
#endif

#ifndef	THREAD_SCRATCH_AREA
#define	THREAD_SCRATCH_AREA		16
#endif

enum thread_state
  {
    THR_RUN,
    THR_READY,
    THR_WAIT,
    THR_DORMANT,
    THR_NONEXISTENT
  };

enum thread_priority
  {
    PRI_HIGH,
    PRI_APPHI,
    PRI_APPLO,
    PRI_LOW,
  };

typedef int8_t thread_id_t;
typedef int8_t thread_priority_t;
typedef void (thread_func)(uint32_t);
typedef thread_func *thread_func_t;

struct thread_stack;

struct thread_control
{
  struct reg regs;
  int8_t state;
  thread_id_t id;
  thread_priority_t priority;
  thread_priority_t native_priority; // for inherit priority

  int8_t wakeup_request;
  bool inherit_priority;
  int8_t monitor_event;
  int8_t monitor_event_signaled;
  const char *name;
  // ready queue
  SIMPLEQ_ENTRY (thread_control) tc_link;
  // monitor wait queue
  SIMPLEQ_ENTRY (thread_control) tc_monitor_lock;
  SIMPLEQ_ENTRY (thread_control) tc_monitor_event;
  // all thread list.
  SLIST_ENTRY (thread_control) t_link;
  // Continuation. (if any)
  continuation_func_t continuation;
  uint32_t continuation_arg;
  // Stack.
  struct thread_stack *tc_stack;
  uint8_t scratch[THREAD_SCRATCH_AREA];	// user scratch area.
} __attribute__((packed, aligned(4)));

typedef struct thread_control *thread_t;

//#ifdef THREAD_PRIVATE
SLIST_HEAD (thread_list, thread_control);
extern struct thread_list thread_list;
thread_t md_thread_create (thread_t, thread_func_t, uint32_t);
void md_thread_continuation_call (continuation_func_t);
void md_thread_continuation_call_with_arg (thread_func_t, uint32_t);
void md_thread_continuation_setup (thread_t);
void md_thread_stack_discard (thread_t);
void md_thread_noreturn_assert (void) __attribute__((noreturn));
//#endif

extern struct thread_control *current_thread;
__BEGIN_DECLS
void thread_system_init (thread_t);
thread_t thread_create_with_stack (uint8_t *, size_t, const char *,
				   thread_func_t, uint32_t);
thread_t thread_create_no_stack (thread_t, const char *, thread_func_t,
				 uint32_t);
int thread_start (thread_t);
int thread_block (continuation_func_t);
int thread_wakeup (thread_t);
int thread_wakeup_once (thread_t);
int thread_priority (thread_t, thread_priority_t);
//#define	THREAD_INHERIT_PRIORITY	current_thread->priority
#define	THREAD_INHERIT_PRIORITY	PRI_HIGH
void thread_priority_inherit_enter (thread_t);
void thread_priority_inherit_exit (thread_t);
int thread_destroy (thread_id_t);
// extern inline can't comiple with -O0
static inline int
thread_rotate_ready_queue (void)
{

  return thread_priority (current_thread, current_thread->priority);
}

#ifdef DEBUG
void md_thread_debug_reg (struct reg *);
void thread_debug_state (uint32_t);
void thread_debug_state_check (void);
void thread_debug_stack_check (void);
#endif
__END_DECLS

#include <sys/thread_stack.h>
#endif
