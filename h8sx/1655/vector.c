
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
// H8SX/1655 interrupt vectors.

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

VECTOR_DECL (nmi, 7);
VECTOR_DECL (trapa0, 8);
VECTOR_DECL (trapa1, 9);
VECTOR_DECL (trapa2, 10);
VECTOR_DECL (trapa3, 11);

VECTOR_DECL (ext_irq0, 64);
VECTOR_DECL (ext_irq1, 65);
VECTOR_DECL (ext_irq2, 66);
VECTOR_DECL (ext_irq3, 67);
VECTOR_DECL (ext_irq4, 68);
VECTOR_DECL (ext_irq5, 69);
VECTOR_DECL (ext_irq6, 70);
VECTOR_DECL (ext_irq7, 71);
VECTOR_DECL (ext_irq8, 72);
VECTOR_DECL (ext_irq9, 73);
VECTOR_DECL (ext_irq10, 74);
VECTOR_DECL (ext_irq11, 75);

VECTOR_DECL (lvd, 78);

VECTOR_DECL (tmr0_cmia, 116);
VECTOR_DECL (tmr0_cmib, 117);
VECTOR_DECL (tmr0_ovf, 118);
VECTOR_DECL (tmr1_cmia, 119);
VECTOR_DECL (tmr1_cmib, 120);
VECTOR_DECL (tmr1_ovf, 121);
VECTOR_DECL (tmr2_cmia, 122);
VECTOR_DECL (tmr2_cmib, 123);
VECTOR_DECL (tmr2_ovf, 124);
VECTOR_DECL (tmr3_cmia, 125);
VECTOR_DECL (tmr3_cmib, 126);
VECTOR_DECL (tmr3_ovf, 127);

VECTOR_DECL (tmr4, 228);
VECTOR_DECL (tmr5, 229);
VECTOR_DECL (tmr6, 230);
VECTOR_DECL (tmr7, 231);

VECTOR_DECL (sci4_eri, 160);
VECTOR_DECL (sci4_rxi, 161);
VECTOR_DECL (sci4_txi, 162);
VECTOR_DECL (sci4_tei, 163);

#define	INTERRUPT_BASE	7

const interrupt_handler_t vector_table[]
__attribute__ ((section (".interrupts"))) =
{
  nmi,		/*  7 0x1c NMI */
  trapa0,	/*  8 0x20 TRAPA#0 */
  trapa1,	/*  9 0x24 TRAPA#1 */
  trapa2,	/* 10 0x28 TRAPA#2 */
  trapa3,	/* 11 0x2c TRAPA#3 */

  /* 64-75 external interrupts. */
  [64 - INTERRUPT_BASE] = ext_irq0,
  [65 - INTERRUPT_BASE] = ext_irq1,
  [66 - INTERRUPT_BASE] = ext_irq2,
  [67 - INTERRUPT_BASE] = ext_irq3,
  [68 - INTERRUPT_BASE] = ext_irq4,
  [69 - INTERRUPT_BASE] = ext_irq5,
  [70 - INTERRUPT_BASE] = ext_irq6,
  [71 - INTERRUPT_BASE] = ext_irq7,
  [72 - INTERRUPT_BASE] = ext_irq8,
  [73 - INTERRUPT_BASE] = ext_irq9,
  [74 - INTERRUPT_BASE] = ext_irq10,
  [75 - INTERRUPT_BASE] = ext_irq11,
  /* 76-77 system reserved */
  [78 - INTERRUPT_BASE] = lvd,	// Low Voltage Detect (IRQ14)
  /* 79-80 system reserved */

  /* 81-255 internal interrupts. */
  [116 - INTERRUPT_BASE] = tmr0_cmia,
  [117 - INTERRUPT_BASE] = tmr0_cmib,
  [118 - INTERRUPT_BASE] = tmr0_ovf,
  [119 - INTERRUPT_BASE] = tmr1_cmia,
  [120 - INTERRUPT_BASE] = tmr1_cmib,
  [121 - INTERRUPT_BASE] = tmr1_ovf,
  [122 - INTERRUPT_BASE] = tmr2_cmia,
  [123 - INTERRUPT_BASE] = tmr2_cmib,
  [124 - INTERRUPT_BASE] = tmr2_ovf,
  [125 - INTERRUPT_BASE] = tmr3_cmia,
  [126 - INTERRUPT_BASE] = tmr3_cmib,
  [127 - INTERRUPT_BASE] = tmr3_ovf,

  [160 - INTERRUPT_BASE] = sci4_eri,
  [161 - INTERRUPT_BASE] = sci4_rxi,
  [162 - INTERRUPT_BASE] = sci4_txi,
  [163 - INTERRUPT_BASE] = sci4_tei,

  [228 - INTERRUPT_BASE] = tmr4,
  [229 - INTERRUPT_BASE] = tmr5,
  [230 - INTERRUPT_BASE] = tmr6,
  [231 - INTERRUPT_BASE] = tmr7,

};

void
null_handler_main (int vector)
{
  iprintf ("null_handler: %d\n", vector);
  while (/*CONSTCOND*/1)
    ;
  // NOTREACHED
}



