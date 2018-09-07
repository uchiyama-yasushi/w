
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

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>

#ifdef INTC_DEBUG
#define	DPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif



void
intc_init ()
{
  // Clear pending interrupt.
  *INTC_IRQRR |= 0;	// read required.

  // Register bank

  *INTC_IBNR = IBNR_BE_IBCR;
#ifdef INTC_BANK_OVERFLOW_EXCEPTION
  *INTC_IBNR |= IBNR_OOVE;
#endif

  //  *INTC_IBCR = 0xfffe;	// Register bank enable all.
  *INTC_IBCR = 0;	// Register bank disable all.
}

enum interrupt_priority
intc_priority (enum interrupt_priority_module module,
	       enum interrupt_priority priority)
{
  // unpack Address + shift
  int shift = module & 0xf;
  int32_t a = (int32_t)module >> 4;	// Sign extension
  volatile uint16_t *r = (volatile uint16_t *)a;
  cpu_status_t s = intr_suspend ();
  uint16_t ipr = *r;

  enum interrupt_priority opri = (ipr >> shift) & IPR_MASK;
  ipr &= ~(0xf << shift);
  ipr |= (priority << shift);
  *r = ipr;
  intr_resume (s);

  DPRINTF ("%x:%d %d->%d\n", r, shift, opri, priority);

  return opri;
}

void
intc_irq_sense (int irq, int sense)
{
  uint16_t shift = irq << 1;
  cpu_status_t s = intr_suspend ();
  *INTC_ICR1 &= ~(0x3 << shift);
  *INTC_ICR1 |= (sense << shift);
  intr_resume (s);
}
