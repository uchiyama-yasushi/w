
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
// SH2A 7262 vectors.

#include <sys/system.h>
#include <sys/console.h>
#include <intc.h>

void null_handler_main (int);

#define	NULL_HANDLER(x)								\
  void null_handler ## x (void) __attribute__ ((interrupt_handler));		\
void										\
null_handler ## x ()								\
{										\
  null_handler_main (x);							\
}

#define	VECTOR_DECL(handler, vector)						\
  NULL_HANDLER (vector)								\
void handler (void) __attribute__ ((weak, alias ("null_handler" #vector)))

VECTOR_DECL (reserved, 0);
VECTOR_DECL (invalid_instruction, 4);
VECTOR_DECL (invalid_instruction_slot, 6);
VECTOR_DECL (address_error_cpu, 9);
VECTOR_DECL (address_error_dma, 10);
VECTOR_DECL (nmi, 11);
VECTOR_DECL (fpu, 13);
VECTOR_DECL (udi, 14);
VECTOR_DECL (bank_overflow, 15);
VECTOR_DECL (bank_underflow, 16);
VECTOR_DECL (div_zero, 17);
VECTOR_DECL (div_overflow, 18);
VECTOR_DECL (trap, 32);

VECTOR_DECL (irq0, 64);
VECTOR_DECL (irq1, 65);
VECTOR_DECL (irq2, 66);
VECTOR_DECL (irq3, 67);
VECTOR_DECL (irq4, 68);
VECTOR_DECL (irq5, 69);
VECTOR_DECL (irq6, 70);
VECTOR_DECL (irq7, 71);

VECTOR_DECL (pint0, 80);
VECTOR_DECL (pint1, 81);
VECTOR_DECL (pint2, 82);
VECTOR_DECL (pint3, 83);
VECTOR_DECL (pint4, 84);
VECTOR_DECL (pint5, 85);
VECTOR_DECL (pint6, 86);
VECTOR_DECL (pint7, 87);

VECTOR_DECL (cmi0, 175);
VECTOR_DECL (cmi1, 176);

VECTOR_DECL (scif6_bri, 256);
VECTOR_DECL (scif6_eri, 257);
VECTOR_DECL (scif6_rxi, 258);
VECTOR_DECL (scif6_txi, 259);

extern void start (void);
extern void stack_start(void);	// XXX kludge hack

const interrupt_handler_t vector_table[]
__attribute__ ((section (".vectors"))) =
{
  [0] = (const interrupt_handler_t)start,
  [1] = (const interrupt_handler_t)stack_start,
  [2] = (const interrupt_handler_t)start,
  [3] = (const interrupt_handler_t)stack_start,
  [4] = invalid_instruction,
  [5] = reserved,
  [6] = invalid_instruction_slot,
  [7] = reserved,
  [8] = reserved,
  [9] = address_error_cpu,
  [10] = address_error_dma,
  [11] = nmi,
  [12] = reserved,
  [13] = fpu,
  [14] = udi,
  [15] = bank_overflow,
  [16] = bank_underflow,
  [17] = div_zero,
  [18] = div_overflow,
  [19] = reserved,
  [20] = reserved,
  [21] = reserved,
  [22] = reserved,
  [23] = reserved,
  [24] = reserved,
  [25] = reserved,
  [26] = reserved,
  [27] = reserved,
  [28] = reserved,
  [29] = reserved,
  [30] = reserved,
  [31] = reserved,
  /* user vector */
  [32] = trap,
  [33] = trap,
  [34] = trap,
  [35] = trap,
  [36] = trap,
  [37] = trap,
  [38] = trap,
  [39] = trap,
  [40] = trap,
  [41] = trap,
  [42] = trap,
  [43] = trap,
  [44] = trap,
  [45] = trap,
  [46] = trap,
  [47] = trap,
  [48] = trap,
  [49] = trap,
  [50] = trap,
  [51] = trap,
  [52] = trap,
  [53] = trap,
  [54] = trap,
  [55] = trap,
  [56] = trap,
  [57] = trap,
  [58] = trap,
  [59] = trap,
  [60] = trap,
  [61] = trap,
  [62] = trap,
  [63] = trap,
  /* External interrupt (IRQ, PINT), Internal module interrupt. */
  [64] = irq0,
  [65] = irq1,
  [66] = irq2,
  [67] = irq3,
  [68] = irq4,
  [69] = irq5,
  [70] = irq6,
  [71] = irq7,

  [80] = pint0,
  [81] = pint1,
  [82] = pint2,
  [83] = pint3,
  [84] = pint4,
  [85] = pint5,
  [86] = pint6,
  [87] = pint7,

  [175] = cmi0,
  [176] = cmi1,

  [256] = scif6_bri,
  [257] = scif6_eri,
  [258] = scif6_rxi,
  [259] = scif6_txi,
};

void
null_handler_main (int vector)
{
  iprintf ("null_handler: %d\n", vector);
  while (/*CONSTCOND*/1)
    ;
  // NOTREACHED
}



