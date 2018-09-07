
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

#include <sys/system.h>
#include <string.h>
#include <stdlib.h>

#include <sys/thread.h>
#include <reg.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <libud01/ud01dev.h>

#include <sys/timer.h>

__BEGIN_DECLS
void push_switch_init (void);
__END_DECLS

STATIC void push_switch_thread (uint32_t);
STATIC void test_thread (uint32_t);

STATIC uint8_t test_tls[THREAD_STACK_SIZE (THREAD_STACKSIZE_DEFAULT)];

struct push_switch_manager
{
  uint8_t tls[THREAD_STACK_SIZE (THREAD_STACKSIZE_DEFAULT)];
  thread_t self;

} push_switch_manager __attribute ((aligned (4)));

thread_t __asm_push_switch_thread;
STATIC thread_id_t test_thread_id;

void
push_switch_init ()
{
  struct push_switch_manager *ps = &push_switch_manager;

  ps->self = thread_create_with_stack (ps->tls, THREAD_STACKSIZE_DEFAULT,
			    "push_switch", push_switch_thread,
			    (uint32_t)ps);
  __asm_push_switch_thread = ps->self;
  thread_start (ps->self);

  thread_t th = thread_create_with_stack (test_tls, THREAD_STACKSIZE_DEFAULT,
			       "test", test_thread, 0);
  test_thread_id = th->id;
  thread_start (th);

  // Use refresh controller as interval timer.
  *RFSHC_RFSHCR = RFSHCR_INTERVAL_TIMER;
  *RFSHC_RTCOR = 0xff;
  *RFSHC_RTCNT = 0;
  *RFSHC_RTMCSR = RTMCSR_CLK_4096 | RTMCSR_CMIE;
}

void
test_thread (uint32_t arg __attribute__((unused)))
{

  while (/*CONSTCOND*/1)
    {
      thread_block (NULL);
    }
}

void
push_switch_thread (uint32_t arg)
{
  struct push_switch_manager  *ps = (struct push_switch_manager *)arg;
  static int h;

  thread_priority (ps->self, PRI_HIGH); // This is interrupt thread.

  while (/*CONSTCOND*/1)
    {
      thread_sleep (NULL);
      //      timer_sleep (0xffff);

      // heart beat.
      h &= 0x7;
      *P4_DR = 1 << h;
      h++;
      switch (push_switch ())
	{
	case PS_0:
	  led (0, 1);
	  assert (0);
	  if (thread_destroy (test_thread_id) == E_OK)
	    printf ("destory success.\n");
	  else
	    printf ("destory falied.\n");
	  break;
	case PS_1:
	  led (1, 1);
	  thread_debug_state (0);
	  break;
	case PS_2:
	  led (0, 0);
	  led (1, 0);
#ifdef DEBUG
	  enum console_device dev;
	  dev = console_output_device;
	  console_output_device = LCD;
	  thread_debug_stack_check ();
	  console_output_device = dev;
#endif
	  break;
	}
    }
}
