
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
#include "keyboard16.h"
#include "caliper_terminal.h"

STATIC void keyboard_thread (uint32_t);

struct keyboard
{
  uint8_t tls[THREAD_STACK_SIZE (THREAD_STACKSIZE_DEFAULT)];
  thread_t self;

} keyboard __attribute ((aligned (4)));

thread_t __asm_keyboard_thread;

void
keyboard_init ()
{
  struct keyboard *kbd = &keyboard;

  kbd->self = thread_create_with_stack (kbd->tls, THREAD_STACKSIZE_DEFAULT, "keyboard",
			     keyboard_thread, (uint32_t)kbd);
  __asm_keyboard_thread = kbd->self;
  thread_start (kbd->self);

  // Use refresh controller as interval timer.
  *RFSHC_RFSHCR = RFSHCR_INTERVAL_TIMER;
  *RFSHC_RTCOR = 201;	// 33msec.
  *RFSHC_RTCNT = 0;
  *RFSHC_RTMCSR = RTMCSR_CLK_4096 | RTMCSR_CMIE;

  // P7 is input port.
  *P2_DDR = 0x00;	//Input
}

void
keyboard_thread (uint32_t arg)
{
  struct keyboard *kbd = (struct keyboard *)arg;
  uint16_t r, old_r, e;
  uint8_t r0, r1;

  thread_priority (kbd->self, PRI_HIGH); // This is interrupt thread.
  old_r = 0;
  while (/*CONSTCOND*/1)
    {
      thread_block (NULL);
      r0 = *P7_DR;
      r1 = *P2_DR;
      r =  r0 | (r1 << 8);
      e = (r ^ old_r) & ~r; // Falling edge

      if (e)
	caliper_terminal_dispatch_command (e);

      old_r = r;
    }
}
