
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
// H8/3664 Standalone program.

#include <sys/system.h>
#include <intc.h>
#include "vector_symbol.h"

const interrupt_handler_t vector_table[]
__attribute ((section (".intvecs"))) =
{
  nmi,		/*  7 0x0e NMI */
  trapa0,	/*  8 0x10 TRAPA#0 */
  trapa1,	/*  9 0x12 TRAPA#1 */
  trapa2,	/* 10 0x14 TRAPA#2 */
  trapa3,	/* 11 0x16 TRAPA#3 */
  abrk,		/* 12 0x18 Adress Break */
  slp,		/* 13 0x1a instruction */
  irq0,		/* 14 0x1c IRQ0 */
  irq1,		/* 15 0x1e IRQ1 */
  irq2,		/* 16 0x20 IRQ2 */
  irq3,		/* 17 0x22 IRQ3 */
  wkp,		/* 18 0x24 Wakeup */
  timer_a,	/* 19 0x26 Timer A */
  null_handler,	/* 20 0x28 */
  timer_w,	/* 21 0x2a Timer W */
  timer_v,	/* 22 0x2c Timer V */
  sci3,		/* 23 0x2e SCI3 */
  i2c,		/* 24 0x30 I2C */
  ad_end,	/* 25 0x32 A/D */
};
