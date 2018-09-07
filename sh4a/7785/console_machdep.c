
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

// Buffered console. device dependent routines. SH7785 SCIF1

#include <sys/system.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/rbuf_nolock.h>

#include <sys/console.h>
#include <reg.h>

#include <cpu.h>//intr_table XXX not cpu.h

STATIC thread_t md_uart_receive_thread;
STATIC rbuf_nolock_t md_uart_receive_rbuf;

STATIC void scif1_intr (void);

void
md_uart_init (thread_t th, rbuf_nolock_t buf)
{

  md_uart_receive_thread = th;
  md_uart_receive_rbuf = buf;

  intr_table[0x780 >> 3] = scif1_intr;
  intr_table[0x7a0 >> 3] = scif1_intr;
}

void
md_uart_intr_enable ()
{
  // Enable 'Receive data full' and 'Receiver error' interrrupt.
  *INT2MSKCR = (1 << 3);	// unmask SCIF1
  *INT2PRI2 |= 0x001f0000;	// SCIF1 highest priority.

  *SCSCR1 |= (1 << 6);	// Receive interrupt enable.
}

void
md_uart_intr_disable ()
{
  // Disable 'Receive data full' and 'Receiver error' interrrupt.
  *INT2MSKR = (1 << 3);		// mask SCIF1
  *INT2PRI2 &= ~0x001f0000;

  *SCSCR1 &= ~(1 << 6);	// Receive interrupt enable.
}

void
scif1_intr ()
{
  uint8_t c;

  c = *SCFRDR1;
  *SCFSR1 &= ~0x2/*RDF*/;

  if (rbuf_nolock_write (md_uart_receive_rbuf, &c, 1) != 1)
    {
#ifdef SCIF_DEBUG
      iprintf ("overflow\n");
#endif
    }

  thread_wakeup_once (md_uart_receive_thread);
}
