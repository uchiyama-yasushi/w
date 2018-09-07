
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
#include <sys/types.h>
#include <sys/console.h>
#include <x86/cpu.h>
#include <x86/gate.h>
#include <x86/exception.h>
#include "gdt.h"

void interrupt_handler_dispatch (uint32_t) __attribute__ ((regparm (1)));

extern const struct vector_config x86_vector_table[];
extern const struct vector_config pcat_vector_table[];

int (*interrupt_handler[16])(void);

void interrupt_init (void);

void
exception_init ()
{
  // Load vector table to IDT.

  // x86 exception.
  x86_exception_init ();
  // PC/AT interrupt handler.
  idt_install (pcat_vector_table);
}

#include <dev/i8259.h>
void
interrupt_init ()
{
  uint16_t mask;

  mask = i8259_get_mask ();
  i8259_enable ();
  i8259_set_mask (mask); // restore.
  i8259_set_mask (0xffff); // disable all.

}

void
interrupt_handler_install (int irq, int (*func) (void))
{

  interrupt_handler[irq] = func;
}

void __attribute__ ((regparm (1)))
interrupt_handler_dispatch (uint32_t irq)
{

  if (interrupt_handler[irq])
    interrupt_handler[irq] ();
  else
    printf ("INTERRUPT %d\n", irq);
}

