
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

// Buffered console. device dependent routines. H8SX/1655 SCI4

#include <sys/system.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/rbuf_nolock.h>

#include <sys/console.h>
#include <reg.h>

__BEGIN_DECLS
void c_sci4 (uint8_t);
__END_DECLS

STATIC thread_t md_uart_receive_thread;
STATIC rbuf_nolock_t md_uart_receive_rbuf;

void
md_uart_init (thread_t th, rbuf_nolock_t buf)
{

  md_uart_receive_thread = th;
  md_uart_receive_rbuf = buf;
  intc_priority (INTPRI_SCI4, INTPRI_7);
}

void
md_uart_intr_enable ()
{
  // Enable 'Receive data full' and 'Receiver error' interrrupt.
  *SCI (4, SCR) |= SCR_RIE;
}

void
md_uart_intr_disable ()
{
  // Disable 'Receive data full' and 'Receiver error' interrrupt.
  *SCI (4, SCR) &= ~SCR_RIE;
}

void
c_sci4 (uint8_t c)
{
  //  iprintf ("[%c]\n", c);

  if (rbuf_nolock_write (md_uart_receive_rbuf, &c, 1) != 1)
    {
#ifdef SCI_DEBUG
      iprintf ("overflow\n");
#endif
    }
  if (*SCI (4, SSR) & SSR_ERR_BITS)
    {
      *SCI (4, SSR) &= ~SSR_ERR_BITS;
#ifdef SCI_DEBUG
      iprintf ("SCI3 Error\n");
#endif
    }

  if (!timer_schedule_func ((void (*)(void *))thread_wakeup_once,
			    md_uart_receive_thread, 255))
    {
      iprintf ("%s: timer busy.\n", __FUNCTION__);
      thread_wakeup_once (md_uart_receive_thread);
    }
}
/*
  1/115200 1bit.
  1/115200*10 (8bit + start bit + stop bit) = .0000868 sec. (86.8usec)
  86.8 * 8 = 694usec (8byte)
 */
