
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

#ifndef _X86_GATE_H_
#define	_X86_GATE_H_
#include <x86/segment.h>

/*
  0	divide by zero
  1	debug
  2	nmi
  3	break point
  4	interrupt on overflow
  5	bound range exceeded
  6	invalid opcode
  7	processor extension not available
 *8	double fault
  9	processor extension segment overrun
 *a	invalid task state segment
 *b	segment not present
 *c	stack segment overrun of not present
 *d	general protection
 #e	page fault.
  f	reserved by intel
  10	coprocessor error
 *11	alignment
  12	machine check
  13-1f	reserved by intel.

  * push error code
  # push error code (special)

    +---------------+
    |     EFLAGS    |-4
    +---------------+
    |     CS        |-8
    +---------------+
    |     EIP       |-12 <-ESP
    +---------------+
    |  Error code   |-16 <-ESP (8,10,11,12,13,14,17) *,#
    +---------------+
    |               |
           ...
    |               | stack_top
    +---+---+---+---+
      3   2   1   0
 */

typedef void (gate_handler)(void);
typedef void (*gate_handler_t)(void);

// User defined IDT configration.
struct vector_config
{
  int number;
  uint8_t type;
  uint8_t nparam;	// Max 31
  uint16_t selector;
  gate_handler_t handler;
};

__BEGIN_DECLS
void idt_install (const struct vector_config *);
void gate (struct gate_descriptor *, uint8_t, uint16_t, gate_handler_t, uint8_t);
void gate_interrupt (struct gate_descriptor *, uint16_t, gate_handler_t);
void gate_trap (struct gate_descriptor *, uint16_t, addr_t);
void gate_call (struct gate_descriptor *, uint16_t, addr_t, int);
void gate_task (struct gate_descriptor *, uint16_t);
void gate_dump_subr (struct gate_descriptor *);
void gate_dump (uint16_t);
__END_DECLS

#endif
