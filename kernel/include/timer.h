
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

#ifndef _TIMER_H_
#define	_TIMER_H_
#ifndef THREAD_DISABLE
#include <sys/thread.h>
#endif

// Delayed dispatch, timer sleep support.
enum timer_counter_match
  {
    COUNTER_STOP,
    COUNTER_RESET,
  };

enum timer_counter_unit
  {
    COUNTER_USEC,
    COUNTER_MSEC,
  };

__BEGIN_DECLS
#ifndef THREAD_DISABLE
bool timer_sleep (timer_counter_t/* usec */);
bool timer_block (timer_counter_t/* usec */, continuation_func_t);
bool timer_sleep_schedule (timer_counter_t interval/* usec */);
void timer_do_wakeup (void);
#endif

// machine dependent implementation.
bool md_timer_start (int/*channel*/, timer_counter_t/*usec*/,
		     enum timer_counter_unit, enum timer_counter_match);
bool md_timer_busy (int/*channel*/);
void md_timer_stop (int/*channel*/);
void timer_debug (void);

bool timer_schedule_func (void (*)(void *), void *, timer_counter_t);/*usec*/
void timer_do_schedule (void);
#endif
