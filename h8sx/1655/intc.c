
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
  uint8_t r = *INTCR;

  // Interrupt cControl mode 2. (use EXR register)
  r &= ~(INTCR_MODE_MASK << INTCR_MODE_SHIFT);
  r |= INTCR_MODE_EXR;

  *INTCR = r;
}

enum interrupt_priority
intc_priority (enum interrupt_priority_module module,
	       enum interrupt_priority priority)
{
  volatile uint16_t *p = IPR_ADDR (module);
  int shift = IPR_SHIFT (module);

  cpu_status_t s = intr_suspend ();
  uint16_t r = *p;
  enum interrupt_priority opri = (r >> shift) & IPR_MASK;
  r &= ~(IPR_MASK << shift);
  r |= priority << shift;
  *p = r;
  intr_resume (s);
  DPRINTF ("%#x(%d) %d->%d\n",  p, shift, opri, priority);

  return opri;
}

void
intc_irq_sense (int irq, int sense)
{
  volatile uint16_t *iscr;
  int shift;
  assert ((irq >= 0 && irq <= 11) || irq == 14);
  assert ((irq != 14) || (sense == ISCR_FALLING_EDGE));

  if (irq < 8)
    {
      iscr = INTC_ISCRL;
      shift = irq << 1;
    }
  else
    {
      iscr = INTC_ISCRH;
      shift = (irq - 8) << 1;
    }

  cpu_status_t s = intr_suspend ();
  *iscr &= ~(0x3 << shift);
  *iscr |= (sense << shift);
  DPRINTF ("%#x sense=%d shift=%d\n",  iscr, sense, shift);

  intr_resume (s);
}
