
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

#include <sys/types.h>
#include <x86/io.h>
#include <dev/i8259.h>

STATIC int OCW3_state;

//#define	BIOS_COMPATIBLE
void
i8259_enable ()
{
  // Master
  // Start ICW
  outb (0x20, ICW1_START | ICW1_IC4);
  // ICW2 (!ICW1_SNGL)
#ifdef BIOS_COMPATIBLE
  outb (0x21, 0x08);
#else
  outb (0x21, 0x20);	// vector address 0x20
#endif
  // ICW3
  outb (0x21, (1 << 2)); // IRQ2 is cascaded.
  // ICW4
  outb (0x21, ICW4_AEOI | ICW4_uPM);

  // Slave
  // Start ICW
  outb (0xa0, ICW1_START | ICW1_IC4);
  // ICW2 (!ICW1_SNGL)
#ifdef BIOS_COMPATIBLE
  outb (0xa1, 0x70);
#else
  outb (0xa1, 0x28);	// vector address 0x28
#endif
  // ICW3 (slave ID)
  outb (0xa1, 0x02);	// my ID is 2
  // ICW4
  outb (0xa1, ICW4_AEOI | ICW4_uPM);// 8086 mode. "Auto EOI mode".

  // Intialization completed.
}

void
i8259_set_mask (uint16_t mask)
{

  // OCW1 mask interrupt.
  outb (0x21, mask);
  outb (0xa1, mask >> 8);
}

uint16_t
i8259_get_mask ()
{
  uint16_t mask;
  uint8_t r;
  inb (0x21, r);
  mask = r;
  inb (0xa1, r);
  mask |= (r << 8);

  return mask;
}

// EOI End Of Interrupt Service.
// When 'Auto EOI mode", these routines are not needed.
void
i8259_eoi_master ()
{
  // non-specific EOI. (Fully Nested Mode && !Automatic EOI Mode)
  outb (0x20, OCW2 | OCW2_EOI);
}

void
i8259_eoi_slave ()
{
  // non-specific EOI. (Fully Nested Mode && !Automatic EOI Mode)
  outb (0x20, OCW2 | OCW2_EOI);
  outb (0xa0, OCW2 | OCW2_EOI);
}

// Inquire i8259 internal registers.
uint16_t
i8259_isr_get ()
{
  uint16_t r;
  uint8_t r0;

  if (OCW3_state != OCW3_READ_ISR)
    outb (0x20, OCW3 | OCW3_READ_ISR);
  inb (0x20, r0);
  r = r0;
  inb (0xa0, r0);
  r |= (r0 << 8);

  return r;
}

uint8_t
i8259_irr_get ()
{
  uint16_t r;
  uint8_t r0;

  if (OCW3_state != OCW3_READ_IRR)
    outb (0x20, OCW3 | OCW3_READ_IRR);
  inb (0x20, r0);
  r = r0;
  inb (0xa0, r0);
  r |= (r0 << 8);

  return r;
}

