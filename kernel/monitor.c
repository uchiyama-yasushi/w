
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

#include <sys/system.h>
#include <string.h>
#include <sys/console.h>
#include <sys/monitor.h>
//#define	MONITOR_DEBUG
#ifdef MONITOR_DEBUG
#define	DPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif
#ifdef MONITOR_VERBOSE
#define	LPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	LPRINTF(fmt, args...)	((void)0)
#endif

SLIST_HEAD (monitor_list, monitor) monitor_list;
#ifdef MONITOR_DEBUG
#define	LOCK2()					\
  cpu_status_t s = intr_suspend ();			\
  DPRINTF ("%d enter \n", current_thread->id)

#define	LOCK()					\
  cpu_status_t s = intr_suspend ();			\
  assert (m->lock_thread == current_thread);	\
  DPRINTF ("%d enter \n", current_thread->id)

#define	UNLOCK()				\
  DPRINTF ("%d exit (%d)\n", current_thread->id, current_thread->priority); \
  intr_resume (s)
#else
#define	LOCK2()					\
  cpu_status_t s = intr_suspend ()
#define	LOCK()					\
  assert (m->lock_thread == current_thread);	\
  cpu_status_t s = intr_suspend ()
#define	UNLOCK()				\
  intr_resume (s)
#endif

void
monitor_init (monitor_t m, const char *name)
{
  static int initialized = FALSE;
  cpu_status_t s = intr_suspend ();

  if (!initialized)
    {
      SLIST_INIT (&monitor_list);
      initialized = TRUE;
    }
  memset (m, 0, sizeof (struct monitor));
  m->name = name;
  m->state = MON_INITIALIZED;
  SIMPLEQ_INIT (&m->lock_queue);
  SIMPLEQ_INIT (&m->event_queue);

  SLIST_INSERT_HEAD (&monitor_list, m, m_link);
  intr_resume (s);
}

void
monitor_enter (monitor_t m)
{
  LOCK2 ();

  assert (m->state);
  if (m->lock_thread)
    {
      // Add to lock wait queue.
      SIMPLEQ_INSERT_TAIL (&m->lock_queue, current_thread, tc_monitor_lock);

      // Inherit priority.
      if (m->lock_thread->priority > current_thread->priority)
	{
	  thread_priority_inherit_enter (m->lock_thread);
	}
      do
	{
	  thread_block (NULL);
	}
      while (m->lock_thread);
      assert (SIMPLEQ_FIRST(&m->lock_queue) == current_thread);
      // Remove myself form lock queue.
      SIMPLEQ_REMOVE_HEAD (&m->lock_queue, tc_monitor_lock);
    }

  m->state = MON_LOCKED;
  m->lock_thread = current_thread;

  UNLOCK ();
}

void
monitor_exit (monitor_t m)
{
  LOCK ();
  bool rotate = FALSE;
  struct thread_control *tc = NULL;

  // Wakeup signaled thread if any.
  // monitor assures event waiting thread can handle resource.
  SIMPLEQ_FOREACH (tc, &m->event_queue, tc_monitor_event)
    {
      if (!tc->monitor_event_signaled)
	continue;
      // Add event waiting thread to highest priority in sake of 'monitor'
      // semantics.
      thread_priority_inherit_enter (tc);
      thread_wakeup (tc);
      rotate = TRUE;
      break;
    }

  // Next, wakeup locked thread from monitor_enter.
  if ((tc = SIMPLEQ_FIRST(&m->lock_queue)) != NULL)
    {
      thread_wakeup (tc);
      rotate = TRUE;
    }

  m->lock_thread = NULL;
  m->state = MON_UNLOCKED;

  // Return to original priority. if inherited.
  thread_priority_inherit_exit (current_thread);

  // Make sure to run signaled thread.
  if (rotate)
    thread_rotate_ready_queue ();

  UNLOCK ();
}

void
monitor_signal (monitor_t m, int event)
{
  LOCK ();
  struct thread_control *tc;

  // monitor don't memorize operation.
  SIMPLEQ_FOREACH (tc, &m->event_queue, tc_monitor_event)
    {
      if (tc->monitor_event & event)
	{
	  // Schedule wakeup. invoked by monitor_exit.
	  tc->monitor_event_signaled = TRUE;
	  DPRINTF ("%d-SIGNAL->%d\n", current_thread->id, tc->id);
	  break;	// Signal only 1 thread.
	}
    }
  m->state = MON_SIGNAL;

  UNLOCK ();
}

void
monitor_wait (monitor_t m, int event)
{
  LOCK ();

  // monitor wait operation always blocked.
  SIMPLEQ_INSERT_TAIL (&m->event_queue, current_thread, tc_monitor_event);

  current_thread->monitor_event = event;
  current_thread->monitor_event_signaled = FALSE;
  m->state = MON_WAIT;

  m->lock_thread = NULL; // unlock
  while (/*CONSTCOND*/1)
    {
      thread_block (NULL);
      // Here, current thread may or may not inherit priority.
      if (m->lock_thread)	// not monitor's wakeup.
	continue;
      if (current_thread->monitor_event_signaled)
	break;
    }
  // lock held.
  m->lock_thread = current_thread;
  current_thread->monitor_event_signaled = FALSE;

  // Remove myself from event queue.
  SIMPLEQ_REMOVE (&m->event_queue, current_thread, thread_control,
		  tc_monitor_event);

  UNLOCK ();
}

void
monitor_event_hook (monitor_t m, int event)
{
  LOCK ();

  // monitor wait operation always blocked.
  SIMPLEQ_INSERT_TAIL (&m->event_queue, current_thread, tc_monitor_event);

  current_thread->monitor_event = event;
  current_thread->monitor_event_signaled = FALSE;
  m->state = MON_WAIT;
  m->lock_thread = NULL;//unlock

  UNLOCK ();
}

void
monitor_event_unhook (monitor_t m)
{
  assert (!m->lock_thread || m->lock_thread == current_thread);
  m->lock_thread = current_thread;
  LOCK ();
  // lock held.
  current_thread->monitor_event_signaled = FALSE;

  // Remove myself from event queue.
  SIMPLEQ_REMOVE (&m->event_queue, current_thread, thread_control,
		  tc_monitor_event);
  UNLOCK ();
}


#ifdef DEBUG
void
monitor_debug ()
{
  cpu_status_t s = intr_suspend ();
  char state[] = "FIULSW"; //Free,Initialized,Unlocked,Locked,Signal,Wait
  struct monitor *m;
  struct thread_control *tc;

  iprintf ("---Monitor---\n");
  SLIST_FOREACH (m, &monitor_list, m_link)
    {
      iprintf ("%c %s\n",state[m->state], m->name);
      if ((tc = m->lock_thread) != NULL)
	{
	  iprintf ("\tlock %d event %x prio %d inherit %d\n",
		   tc->id, tc->monitor_event,
		   tc->native_priority, tc->priority);
	}
      iprintf ("\tlock : ");
      SIMPLEQ_FOREACH (tc, &m->lock_queue, tc_monitor_lock)
	iprintf ("%d(%s) ", tc->id, tc->name);
      iprintf ("\n\tevent: ");
      SIMPLEQ_FOREACH (tc, &m->event_queue, tc_monitor_event)
	iprintf ("%d(%s) ", tc->id, tc->name);
      iprintf ("\n");
    }
  intr_resume (s);
}
#endif //DEBUG
