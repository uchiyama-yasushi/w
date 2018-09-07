
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

#include <sys/system.h>
#include <vector_virtual.h>
#include "vector_symbol.h"

// For ROM-monitor.

// Virtual vector table. located anywhere
const interrupt_handler_t vector_table_virtual[]
__attribute ((section (".intvecs_ram"))) =
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

// Real vector table. located ROM
__BEGIN_DECLS
void jmp07 (void);
void jmp08 (void);
void jmp09 (void);
void jmp10 (void);
void jmp11 (void);
void jmp12 (void);
void jmp13 (void);
void jmp14 (void);
void jmp15 (void);
void jmp16 (void);
void jmp17 (void);
void jmp18 (void);
void jmp19 (void);
void jmp20 (void);
void jmp21 (void);
void jmp22 (void);
void jmp23 (void);
void jmp24 (void);
void jmp25 (void);
void jmp26 (void);
void jmp27 (void);
void jmp28 (void);
void jmp29 (void);
void jmp30 (void);
void jmp31 (void);
void jmp32 (void);
void jmp33 (void);
void jmp34 (void);
void jmp35 (void);
void jmp36 (void);
void jmp37 (void);
void jmp38 (void);
void jmp39 (void);
void jmp40 (void);
void jmp41 (void);
void jmp42 (void);
void jmp43 (void);
void jmp44 (void);
void jmp45 (void);
void jmp46 (void);
void jmp47 (void);
void jmp48 (void);
void jmp49 (void);
void jmp50 (void);
void jmp51 (void);
void jmp52 (void);
void jmp53 (void);
void jmp54 (void);
void jmp55 (void);
void jmp56 (void);
void jmp57 (void);
void jmp58 (void);
void jmp59 (void);
void jmp60 (void);
__END_DECLS

const interrupt_handler_t vector_table_rom[]
__attribute ((section (".intvecs_rom"))) =
{
  jmp07, /*  7 0x1c NMI */
  jmp08, /*  8 0x20 TRAPA#0 */
  jmp09, /*  9 0x24 TRAPA#1 */
  jmp10, /* 10 0x28 TRAPA#2 */
  jmp11, /* 11 0x2c TRAPA#3 */
  jmp12, /* 12 0x30 IRQ0 */
  jmp13, /* 13 0x34 IRQ1 */
  jmp14, /* 14 0x38 IRQ2 */
  jmp15, /* 15 0x3c IRQ3 */
  jmp16, /* 16 0x40 IRQ4 */
  jmp17, /* 17 0x44 IRQ5 */
  jmp18, /* 18 0x48 */
  jmp19, /* 19 0x4c */
  jmp20, /* 20 0x50 WOVI */
  jmp21, /* 21 0x54 CMI */
  jmp22, /* 22 0x58 */
  jmp23, /* 23 0x5c */
  jmp24, /* 24 0x60 IMIA0 */
  jmp25, /* 25 0x64 IMIB0 */
  jmp26, /* 26 0x68 OVI0 */
  jmp27, /* 27 0x6c */
  jmp28, /* 28 0x70 IMIA1 */
  jmp29, /* 29 0x74 IMIB1 */
  jmp30, /* 30 0x78 OVI1 */
  jmp31, /* 31 0x7c */
  jmp32, /* 32 0x80 IMIA2 */
  jmp33, /* 33 0x84 IMIB2 */
  jmp34, /* 34 0x88 OVI2 */
  jmp35, /* 35 0x8c */
  jmp36, /* 36 0x90 IMIA3 */
  jmp37, /* 37 0x94 IMIB3 */
  jmp38, /* 38 0x98 OVI3 */
  jmp39, /* 39 0x9c */
  jmp40, /* 40 0xa0 IMIA4 */
  jmp41, /* 41 0xa4 IMIB4 */
  jmp42, /* 42 0xa8 OVI4 */
  jmp43, /* 43 0xac */
  jmp44, /* 44 0xb0 DEND0A */
  jmp45, /* 45 0xb4 DEND0B */
  jmp46, /* 46 0xb8 DEND1A */
  jmp47, /* 47 0xbc DEND1B */
  jmp48, /* 48 0xc0 */
  jmp49, /* 49 0xc4 */
  jmp50, /* 50 0xc8 */
  jmp51, /* 51 0xcc */
  jmp52, /* 52 0xd0 ERI0 */
  jmp53, /* 53 0xd4 RXI0 */
  jmp54, /* 54 0xd8 TXI0 */
  jmp55, /* 55 0xdc TEI0 */
  jmp56, /* 56 0xe0 ERI1 */
  jmp57, /* 57 0xe4 RXI1 */
  jmp58, /* 58 0xe8 TXI1 */
  jmp59, /* 59 0xec TEI1 */
  jmp60, /* 60 0xf0 ADI  */
};
