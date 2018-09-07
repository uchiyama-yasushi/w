
/*-
 * Copyright (c) 2008 UCHIYAMA Yasushi.  All rights reserved.
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
#include <sys/timer.h>
#include <sys/console.h>

#ifndef THREAD_DISABLE
#include <sys/thread.h>
#endif	// !THREAD_DISABLE

__BEGIN_DECLS
volatile uint32_t __timer_counter;
__END_DECLS

struct timer_dispatch
{
  void (*func)(void *);
  void *arg;
#ifndef THREAD_DISABLE
  thread_t id;
#endif
} timer_dispatch[3];

#ifndef THREAD_DISABLE
void
timer_debug ()
{
  int i;

  for (i = 0; i < 3; i++)
    {
      printf ("TIMER%d tid=%d\n", i, timer_dispatch[i].id->id);
    }
}

// Timer sleep
bool
timer_sleep (timer_counter_t interval)	/* usec */
{
  if (!timer_sleep_schedule (interval))
    return FALSE;

  thread_block (NULL);
  return TRUE;
}

bool
timer_block (timer_counter_t interval/* usec */, continuation_func_t cont)
{
  if (!timer_sleep_schedule (interval))
    return FALSE;

  thread_block (cont);
  return TRUE;
}

bool
timer_sleep_schedule (timer_counter_t interval/* usec */)
{
  struct timer_dispatch *t = timer_dispatch + 1;

  if (t->id)
    {
      iprintf ("%s:busy. tid=%d %p %p\n", __FUNCTION__, timer_dispatch[1].id->id,
	       timer_dispatch, t);
      return FALSE;
    }

  md_timer_start (1, interval, COUNTER_USEC, COUNTER_STOP);
  t->id = current_thread;

  return TRUE;
}

void
timer_do_wakeup ()
{
  struct timer_dispatch *t = timer_dispatch + 1;
  thread_t id = t->id;

  assert (id);

  t->id = 0;
  thread_wakeup (id);
}
#endif // !THREAD_DISABLE

// Delayed dispatch
// Schedule delayed dispatch with function.
bool
timer_schedule_func (void (*func)(void *), void *arg, timer_counter_t interval)
{
  struct timer_dispatch *t = timer_dispatch + 0;

  if (t->func)
    return FALSE;	// busy;

  t->func = func;
  t->arg = arg;

  return md_timer_start (0, interval, COUNTER_USEC, COUNTER_STOP);
}

// Delayed dispatch.
void
timer_do_schedule ()
{
  struct timer_dispatch *t = timer_dispatch + 0;
  assert (t->func);

  t->func (t->arg);
  t->func = NULL;
  t->arg = NULL;
}
