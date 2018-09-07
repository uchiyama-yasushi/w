
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
#include <x86/segment.h>
#include <x86/gate.h>
#include "gdt.h"

gate_handler int20_irq0;
gate_handler int21_irq1;
gate_handler int22_irq2;
gate_handler int23_irq3;
gate_handler int24_irq4;
gate_handler int25_irq5;
gate_handler int26_irq6;
gate_handler int27_irq7;
gate_handler int28_irq8;
gate_handler int29_irq9;
gate_handler int2a_irq10;
gate_handler int2b_irq11;
gate_handler int2c_irq12;
gate_handler int2d_irq13;
gate_handler int2e_irq14;
gate_handler int2f_irq15;

const struct vector_config pcat_vector_table[] =
  {
    { 0x20, SYS_INTRGATE, 0, GDT_CODE32, int20_irq0 },
    { 0x21, SYS_INTRGATE, 0, GDT_CODE32, int21_irq1 },
    { 0x22, SYS_INTRGATE, 0, GDT_CODE32, int22_irq2 },
    { 0x23, SYS_INTRGATE, 0, GDT_CODE32, int23_irq3 },
    { 0x24, SYS_INTRGATE, 0, GDT_CODE32, int24_irq4 },
    { 0x25, SYS_INTRGATE, 0, GDT_CODE32, int25_irq5 },
    { 0x26, SYS_INTRGATE, 0, GDT_CODE32, int26_irq6 },
    { 0x27, SYS_INTRGATE, 0, GDT_CODE32, int27_irq7 },
    { 0x28, SYS_INTRGATE, 0, GDT_CODE32, int28_irq8 },
    { 0x29, SYS_INTRGATE, 0, GDT_CODE32, int29_irq9 },
    { 0x2a, SYS_INTRGATE, 0, GDT_CODE32, int2a_irq10 },
    { 0x2b, SYS_INTRGATE, 0, GDT_CODE32, int2b_irq11 },
    { 0x2c, SYS_INTRGATE, 0, GDT_CODE32, int2c_irq12 },
    { 0x2d, SYS_INTRGATE, 0, GDT_CODE32, int2d_irq13 },
    { 0x2e, SYS_INTRGATE, 0, GDT_CODE32, int2e_irq14 },
    { 0x2f, SYS_INTRGATE, 0, GDT_CODE32, int2f_irq15 },
    { 0x100, 0, 0, 0, 0 }, // Terminator.
  };
