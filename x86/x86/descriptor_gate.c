
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
#include <x86/segment.h>
#include <x86/gate.h>
#include <sys/console.h>

void
gate (struct gate_descriptor *g, uint8_t gate_type, uint16_t selector,
      gate_handler_t func, uint8_t nparam)
{

  g->selector		= selector;
  g->offset_0_15	= (addr_t)func & 0xffff;
  g->offset_16_31	= ((addr_t)func >> 16) & 0xffff;
  g->parameter_0_4	= nparam;
  g->access_byte	= PRESENT | gate_type;
}

void
gate_interrupt (struct gate_descriptor *g, uint16_t selector,
		gate_handler_t func)
{

  g->selector		= selector;
  g->offset_0_15	= (addr_t)func & 0xffff;
  g->offset_16_31	= ((addr_t)func >> 16) & 0xffff;
  g->parameter_0_4	= 0;	// unused
  g->access_byte	= PRESENT | SYS_INTRGATE;
}

void
gate_trap (struct gate_descriptor *g, uint16_t selector, addr_t func)
{

  g->selector		= selector;
  g->offset_0_15	= func & 0xffff;
  g->offset_16_31	= (func >> 16) & 0xffff;
  g->parameter_0_4	= 0;	// unused
  g->access_byte	= PRESENT | SYS_TRAPGATE;
}

void
gate_call (struct gate_descriptor *g, uint16_t selector, addr_t func, int nparam)
{

  g->selector		= selector;
  g->offset_0_15	= func & 0xffff;
  g->offset_16_31	= (func >> 16) & 0xffff;
  g->parameter_0_4	= nparam;	// MAX 31
  g->access_byte	= PRESENT | SYS_CALLGATE;
}

void
gate_task (struct gate_descriptor *g, uint16_t selector)
{

  g->selector		= selector;
  g->offset_0_15	= 0;	// unused
  g->offset_16_31	= 0;	// unused
  g->parameter_0_4	= 0;	// unused
  g->access_byte	= PRESENT | SYS_TASKGATE;
}

void
gate_dump (uint16_t selector)
{
  union descriptor *d = descriptor (selector);

  printf ("[GATE %x] ", selector);
  gate_dump_subr (&d->gate);
}

void
gate_dump_subr (struct gate_descriptor *g)
{
  uint8_t access_byte = g->access_byte;

  if (access_byte & PRESENT)
    printf ("present ");
  printf ("DPL:%d ", access_byte >> 5 & 0x3);
  switch (access_byte & 0xf)
    {
    case SYS_TRAPGATE:
      printf ("TRAP ");
      break;
    case SYS_INTRGATE:
      printf ("INTR ");
      break;
    case SYS_TASKGATE:
      printf ("TASK ");
      break;
    case SYS_CALLGATE:
      printf ("CALL param %d", g->parameter_0_4);
      break;
    default:
      printf ("invalid gate type ");
      break;
    }
  printf ("func: %x:%x\n", g->selector, g->offset_0_15 | (g->offset_16_31 << 16));

}
