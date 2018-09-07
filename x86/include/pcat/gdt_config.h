
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

#ifndef _PCAT_GDT_H_
#define	_PCAT_GDT_H_

struct gdt_config gdt_config [] = {
  // 4G flat.
  { 0x0, 0x100000, SIZE_UNIT_4KB, CODE_32, CODE_SEGMENT, "CODE32" },
  { 0x0, 0x100000, SIZE_UNIT_4KB, CODE_32, DATA_SEGMENT, "DATA32" },
  // For Real-mode.
#ifdef BOOTSTRAP_GDT
  { 0x0, 0x10000, SIZE_UNIT_1B,  CODE_16, CODE_SEGMENT, "CODE16" },
  { 0x0, 0x10000, SIZE_UNIT_1B,  CODE_16, DATA_SEGMENT, "DATA16" },
#else
  { 0x80000, 0x10000, SIZE_UNIT_1B, CODE_32, CODE_SEGMENT, "CODE32_BIOS" },
  { 0x80000, 0x10000, SIZE_UNIT_1B, CODE_32, DATA_SEGMENT, "DATA32_BIOS" },
  { 0x80000, 0x10000, SIZE_UNIT_1B, CODE_16, CODE_SEGMENT, "CODE16" },
  { 0x80000, 0x10000, SIZE_UNIT_1B, CODE_16, DATA_SEGMENT, "DATA16" },
  { 0x0, 0x1000, SIZE_UNIT_1B, CODE_32, DATA_SEGMENT, "TEST32" },
  { 0x0, 0x0, SIZE_UNIT_1B, SYSSEG, TASK_SEGMENT, "TSS_KERNEL" },
  { 0x0, 0x0, 0x0, 0x0, 0x0, "GATE_KERNEL" },
  { 0x0, 0x0, SIZE_UNIT_1B, SYSSEG, TASK_SEGMENT, "TSS_DOUBLE_FAULT" },
  { 0x0, 0x0, 0x0, 0x0, 0x0, "GATE_DOUBLE_FAULT" },
  { 0x0, 0x0, SIZE_UNIT_1B, SYSSEG, TASK_SEGMENT, "TSS_INVALID_TSS" },
  { 0x0, 0x0, 0x0, 0x0, 0x0, "GATE_INVALID_TSS" },
  { 0x0, 0x0, SIZE_UNIT_1B, SYSSEG, TASK_SEGMENT, "TSS_STACK_OVERRUN" },
  { 0x0, 0x0, 0x0, 0x0, 0x0, "GATE_STACK_OVERRUN" },
  { 0x0, 0x0, SIZE_UNIT_1B, SYSSEG, TASK_SEGMENT, "TSS_TASK0" },
  { 0x0, 0x0, 0x0, 0x0, 0x0, "GATE_TASK0" },
  { 0x0, 0x0, SIZE_UNIT_1B, SYSSEG, TASK_SEGMENT, "TSS_TASK3" },
  { 0x0, 0x0, 0x0, 0x0, 0x0, "GATE_TASK3" },
  { 0x0, 0x0, SIZE_UNIT_1B, SYSSEG, LDT_SEGMENT, "LDT0" },
  { 0x0, 0x0, SIZE_UNIT_1B, SYSSEG, LDT_SEGMENT, "LDT1" },
  { 0x0, 0x0, SIZE_UNIT_1B, SYSSEG, LDT_SEGMENT, "LDT2" },
  { 0x0, 0x0, SIZE_UNIT_1B, SYSSEG, LDT_SEGMENT, "LDT3" },
  { 0x0, 0x0, SIZE_UNIT_1B, SYSSEG, LDT_SEGMENT, "LDT4" },
  { 0x0, 0x0, 0x0, 0x0, 0x0, "GATE_SYSTEMCALL" },
#endif
};
#endif
