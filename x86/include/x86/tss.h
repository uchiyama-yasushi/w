
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

#ifndef _X86_TSS_H_
#define	_X86_TSS_H_
struct tss
{
  uint16_t backlink;	uint16_t pad0;
  uint32_t esp0;
  uint16_t ss0;		uint16_t pad1;
  uint32_t esp1;
  uint16_t ss1;		uint16_t pad2;
  uint32_t esp2;
  uint16_t ss2;		uint16_t pad3;
  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;
  uint16_t es;		uint16_t pad4;
  uint16_t cs;		uint16_t pad5;
  uint16_t ss;		uint16_t pad6;
  uint16_t ds;		uint16_t pad7;
  uint16_t fs;		uint16_t pad8;
  uint16_t gs;		uint16_t pad9;
  uint16_t ldt;		uint16_t pada;
  uint16_t t;		uint16_t iomap;
#define	TSS_OSDEP_MAX	64
  char osdep[TSS_OSDEP_MAX];	// OS dependent region.
  //#define	IO_MASK_MAX	0x800
#define	TSS_IO_MASK_MAX	1
  uint32_t io_mask[TSS_IO_MASK_MAX];
} __attribute ((packed));

// User defined config parameters.
struct segment_config
{
  void *start;
  size_t size;
  uint16_t priority;
  uint16_t segment;
};

struct tss_config
{
  struct tss *tss;
  uint16_t tss_segment;
  uint16_t gate_selector;
  int priority;
  uint16_t code_segment;
  uint16_t data_segment;
  struct segment_config ldt;
  struct segment_config stack;
  struct segment_config stack_privilege;
  uint32_t eflags;
  void (*func) (void);
  const char *name;
};

__BEGIN_DECLS
void tss_install_gdt (struct tss *, uint16_t, uint16_t);
void tss_set_tr (uint16_t);
uint16_t tss_get_tr (void);
void tss_setup (const struct tss_config *);
#define	tss_call(selector)	__asm volatile ("lcall %0, $0":: "i"(selector));
void tss_dump (uint16_t);
void tss_dump_backlink (void);
void tss_dump_current (void);
struct tss *tss_get_context (uint16_t);
void *tss_get_current_stack_top0 ();

// User interface.
void task_init (void);
void task0_install (void (*)(void));
void task3_install (void (*)(void));

__END_DECLS
#endif
