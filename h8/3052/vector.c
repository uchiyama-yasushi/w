
/*-
 * Copyright (c) 2008-2009 UCHIYAMA Yasushi.  All rights reserved.
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

// For standalone program.
#include <sys/system.h>
#include <reg.h>
#include <vector_symbol.h>

const interrupt_handler_t vector_table[]
__attribute ((section (".intvecs"))) =
{
  nmi,		/*  7 0x1c NMI */
  trapa0,	/*  8 0x20 TRAPA#0 */
  trapa1,	/*  9 0x24 TRAPA#1 */
  trapa2,	/* 10 0x28 TRAPA#2 */
  trapa3,	/* 11 0x2c TRAPA#3 */
  irq0,		/* 12 0x30 IRQ0 */
  irq1,		/* 13 0x34 IRQ1 */
  irq2,		/* 14 0x38 IRQ2 */
  irq3,		/* 15 0x3c IRQ3 */
  irq4,		/* 16 0x40 IRQ4 */
  irq5,		/* 17 0x44 IRQ5 */
  null_handler,	/* 18 0x48 */
  null_handler,	/* 19 0x4c */
  wdt_ovf,	/* 20 0x50 WOVI */
  rfshc_cmi,	/* 21 0x54 CMI */
  null_handler,	/* 22 0x58 */
  null_handler,	/* 23 0x5c */
  itu0_a,	/* 24 0x60 IMIA0 */
  itu0_b,	/* 25 0x64 IMIB0 */
  itu0_ovf,	/* 26 0x68 OVI0 */
  null_handler,	/* 27 0x6c */
  itu1_a,	/* 28 0x70 IMIA1 */
  itu1_b,	/* 29 0x74 IMIB1 */
  itu1_ovf,	/* 30 0x78 OVI1 */
  null_handler,	/* 31 0x7c */
  itu2_a,	/* 32 0x80 IMIA2 */
  itu2_b,	/* 33 0x84 IMIB2 */
  itu2_ovf,	/* 34 0x88 OVI2 */
  null_handler,	/* 35 0x8c */
  itu3_a,	/* 36 0x90 IMIA3 */
  itu3_b,	/* 37 0x94 IMIB3 */
  itu3_ovf,	/* 38 0x98 OVI3 */
  null_handler,	/* 39 0x9c */
  itu4_a,	/* 40 0xa0 IMIA4 */
  itu4_b,	/* 41 0xa4 IMIB4 */
  itu4_ovf,	/* 42 0xa8 OVI4 */
  null_handler,	/* 43 0xac */
  dmac0_a,	/* 44 0xb0 DEND0A */
  dmac0_b,	/* 45 0xb4 DEND0B */
  dmac1_a,	/* 46 0xb8 DEND1A */
  dmac1_b,	/* 47 0xbc DEND1B */
  null_handler,	/* 48 0xc0 */
  null_handler,	/* 49 0xc4 */
  null_handler,	/* 50 0xc8 */
  null_handler,	/* 51 0xcc */
  sci0_eri,	/* 52 0xd0 ERI0 */
  sci0_rxi,	/* 53 0xd4 RXI0 */
  sci0_txi,	/* 54 0xd8 TXI0 */
  sci0_tei,	/* 55 0xdc TEI0 */
  sci1_eri,	/* 56 0xe0 ERI1 */
  sci1_rxi,	/* 57 0xe4 RXI1 */
  sci1_txi,	/* 58 0xe8 TXI1 */
  sci1_tei,	/* 59 0xec TEI1 */
  ad_end,	/* 60 0xf0 ADI  */
};
