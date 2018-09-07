
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

#ifndef _X86_SEGMENT_H_
#define	_X86_SEGMENT_H_

#define	IDT_MAX	0x100
#define	LDT_MAX	0x2000	// 2^13
#define	GDT_MAX	0x2000

/*
  GDT ... code, data, stack | call, task,            | tss, ldt
  LDT ... code, data, stack | call, task,            |
  IDT ...                   |       task, intr, trap |
 */

// Access byte (Lower attribute filed) defines.
#define	PRESENT			0x80

// DPL
#define	DPL_MASK		0x60
#define	DPL_SHIFT		5

// S (Descriptor Type) bit
#define	SEGMENT_SYSTEM		0x00
#define	SEGMENT_APPLICATION	0x10

// Code/Text Segment.
#define	APP_CODE		0x08
#define	APP_DATA		0x00
/*
  Data:
  E Lower extension. (Stack)
  W Writable
  A Access
  Code:
  C Conforming (omit privileged level check.)
  R Readable
  A Access
*/
#define	DATA_R			0x00
#define	DATA_RW			0x02
#define	DATA_R_STACK		0x04
#define	DATA_RW_STACK		0x06

// C ... Conforming. omit privileged level check
#define	CODE_X			0x00
#define	CODE_RX			0x02
#define	CODE_XC			0x04
#define	CODE_RXC		0x06

// Type filed for System Segment.
#define	SYS_LDT			0x02
#define	SYS_TSS			0x09
#define	SYS_TSS_BUSY_BIT	0x02
#define	SYS_TASKGATE		0x05
#define	SYS_CALLGATE		0x0c
#define	SYS_INTRGATE		0x0e
#define	SYS_TRAPGATE		0x0f

#define	TASK_SEGMENT	(PRESENT | SEGMENT_SYSTEM | SYS_TSS)
#define	TASK_SEGMENT_INVALID	(SEGMENT_SYSTEM | SYS_TSS)
#define	LDT_SEGMENT	(PRESENT | SEGMENT_SYSTEM | SYS_LDT)

#define	CODE_SEGMENT	(PRESENT | SEGMENT_APPLICATION | APP_CODE | CODE_RX)
#define	DATA_SEGMENT	(PRESENT | SEGMENT_APPLICATION | APP_DATA | DATA_RW)
#define	DATA_SEGMENT_INVALID	(SEGMENT_APPLICATION | APP_DATA | DATA_RW)
#define	STACK_SEGMENT	(PRESENT | SEGMENT_APPLICATION | APP_DATA | DATA_RW_STACK)

// Upper Attribute field.
// Granularity bit
#define	SIZE_UNIT_4KB		0x80
#define	SIZE_UNIT_1B		0x00
// Default bit. application descriptor only.
#define	CODE_32			0x40
#define	CODE_16			0x00
#define	SYSSEG			0x00

struct application_descriptor // including system descriptor
{
  uint16_t size_0_15;
  uint16_t base_0_15;
  uint8_t base_16_23;
  uint8_t access_byte;
  uint8_t size_16_19_GD;
  uint8_t base_24_31;
} __attribute ((packed));

/*
  System descriptor type filed.
  2 ... LDT
  5 ... Task gate
  9 ... 386TSS available
  b ... 386TSS busy.
  c ... 386call gate
  e ... 386intr gate
  f ... 386trap gate
 */

struct gate_descriptor // call gate, interrupt gate, trap gate.
{
  uint16_t offset_0_15;
  uint16_t selector;
  uint8_t parameter_0_4;// # of parameters. call gate only.
  uint8_t access_byte;
  /*
    System:
    P DPL 0 1 1 1 1 trap gate (don't change IF)
    P DPL 0 1 1 1 0 interrupt gate (clear IF)
    P DPL 0 1 1 0 0 call gate
    P DPL 0 0 1 0 1 task gate
    Application:
    P DPL 1 0 . . . data/stack
    P DPL 1 1 . . . code
   */
  uint16_t offset_16_31;
} __attribute ((packed));

union descriptor
{
  struct application_descriptor desc;
  struct gate_descriptor gate;
  uint16_t u16[4];
  uint32_t u32[2];
};

struct lgdt_arg
{
  uint16_t limit;
  uint32_t base;
} __attribute ((packed));

struct lidt_arg
{
  uint16_t limit;
  uint32_t base;
} __attribute ((packed));

extern struct lidt_arg protectmode_lidt_arg;
extern struct lidt_arg realmode_lidt_arg;	// for BIOS call.

// Application specific configuration.

struct gdt_config
{
  uint32_t base;
  uint32_t size;
  uint8_t unit;
  uint8_t mode;
  uint8_t type;
  const char *name;
};

/*
 * Selector
 */
#define	SELECTOR_RPL_MASK	0x3
#define	SELECTOR_TI		0x4

__BEGIN_DECLS
union descriptor *descriptor (int);
void descriptor_set_base (struct application_descriptor *, uint32_t);
void descriptor_set_size (struct application_descriptor *, uint32_t);
uint32_t descriptor_get_base (struct application_descriptor *);
uint32_t descriptor_get_size (struct application_descriptor *);
void descriptor_set_priority (union descriptor *, int);
int descriptor_get_priority (union descriptor *);
__END_DECLS
#endif
