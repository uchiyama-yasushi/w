
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

#ifndef _MONITOR_H_
#define	_MONITOR_H_
#include <sys/thread.h>

enum monitor_state
  {
    MON_FREE,
    MON_INITIALIZED,
    MON_UNLOCKED,
    MON_LOCKED,
    MON_SIGNAL,
    MON_WAIT,
  };

SIMPLEQ_HEAD (tc_queue, thread_control);

typedef struct monitor
{
  enum monitor_state state;
  struct tc_queue lock_queue;
  struct tc_queue event_queue;
  const char *name;
  thread_t lock_thread;
  SLIST_ENTRY (monitor) m_link;
} *monitor_t;

__BEGIN_DECLS
void monitor_init (monitor_t, const char *);
void monitor_enter (monitor_t);
void monitor_exit (monitor_t);
void monitor_signal (monitor_t, int);
void monitor_wait (monitor_t, int);

void monitor_event_hook (monitor_t, int);
void monitor_event_unhook (monitor_t);

#ifdef DEBUG
void monitor_debug (void);
#endif
__END_DECLS
#endif
