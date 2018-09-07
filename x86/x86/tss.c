
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

#include <sys/system.h>
#include <sys/types.h>
#include <sys/console.h>
#include <x86/segment.h>
#include <x86/gate.h>
#include <x86/cpu.h>
#include <x86/tss.h>

#include <string.h>
#include <stddef.h>	//offsetof ()

void
tss_install_gdt (struct tss *tss, uint16_t desc_idx, uint16_t gate_idx)
{
  union descriptor *d;

  // TSS itself.
  d = descriptor (desc_idx);
  descriptor_set_base (&d->desc, (uint32_t)tss);
  descriptor_set_size (&d->desc, sizeof (struct tss));

  // Corresponding gate.
  d = descriptor (gate_idx);
  gate_task (&d->gate, desc_idx);
}

void
tss_set_tr (uint16_t selector)
{

  __asm volatile ("ltr	%0":: "a"(selector));
}

uint16_t
tss_get_tr ()
{
  uint16_t selector;

  __asm volatile ("str %0": "=g"(selector));

  return selector;
}

uint16_t
ldt_setup (const struct tss_config *conf)
{
  union descriptor *d;
  union descriptor *ldt = (union descriptor *)conf->ldt.start;
  int priority = conf->priority;

  // LDT itself.
  d = descriptor (conf->ldt.segment);
  descriptor_set_base (&d->desc, (uint32_t)conf->ldt.start);
  descriptor_set_size (&d->desc, conf->ldt.size);
  descriptor_set_priority (d, priority);

  // Copy code segment.
  d = descriptor (conf->code_segment);
  ldt[1] = *d;
  descriptor_set_priority (ldt + 1, priority);// Set DPL
  // Copy data segment. (including stack)
  d = descriptor (conf->data_segment);
  ldt[2] = *d;
  descriptor_set_priority (ldt + 2, priority);// Set DPL

  // Copy stack segment for privilege change. (If any.)
  if (conf->stack_privilege.segment)
    {
      d = descriptor (conf->stack_privilege.segment);
      ldt[3] = *d;
      descriptor_set_priority (ldt + 3, conf->stack_privilege.priority);
    }

  return conf->ldt.segment;
}

void
tss_setup (const struct tss_config *conf)
{
  struct tss *tss = conf->tss;
  int priority = conf->priority;
  const struct segment_config *seg;

  printf ("%s:[%s] stack:0x%x-0x%x EFLAGS=0x%x priority:%d\n",
	  __FUNCTION__, conf->name, conf->stack.start, conf->stack.size,
	  conf->eflags, conf->priority);

  memset (tss, 0, sizeof *tss);

  // Construct my LDT.
  tss->ldt = ldt_setup (conf);

  // Stack for privilege change.
  seg = &conf->stack_privilege;
  tss->esp0 = (uint32_t)seg->start + seg->size - 16;
  tss->ss0 = 24 | SELECTOR_TI | seg->priority;
  tss->ss1 = tss->ss2 = 0;	// unused.
  tss->esp1= tss->esp2 = 0;	// unused.

  // Normal stack.
  seg = &conf->stack;
  tss->eflags = conf->eflags;
  tss->esp = (uint32_t)seg->start + seg->size - 16;

  // Code/Data segment.
  tss->cs = 8 | SELECTOR_TI | priority;
  tss->ds = tss->es = tss->fs  = tss->gs = tss->ss =
    16 | SELECTOR_TI | priority;

  // Entry point.
  tss->eip = (uint32_t)conf->func;

  // IOPL
  tss->iomap = offsetof (struct tss, io_mask);
  tss->io_mask[TSS_IO_MASK_MAX - 1] |= 0xff000000;	// Marker.

  // OS depenedent.
  strncpy (tss->osdep, conf->name, TSS_OSDEP_MAX);
  tss->osdep[TSS_OSDEP_MAX - 1] = '\0';
}

struct tss *
tss_get_context (uint16_t selector)
{
  union descriptor *d = descriptor (selector);

  if ((d->desc.access_byte & ~(PRESENT | SYS_TSS_BUSY_BIT)) != SYS_TSS)
    {
      printf ("%s: not a TSS descriptor\n", __FUNCTION__);
      return NULL;
    }

  return (struct tss *)descriptor_get_base (&d->desc);
}

void *
tss_get_current_stack_top0 ()
{
  uint16_t selector = tss_get_tr ();
  union descriptor *d = descriptor (selector);
  struct tss *tss = (struct tss *)descriptor_get_base (&d->desc);

  return (void *)tss->esp0;
}

void
tss_dump (uint16_t selector)
{
  union descriptor *d;
  uint32_t base, size;
  struct tss *tss;

  d = descriptor (selector);
  base = descriptor_get_base (&d->desc);
  size = descriptor_get_size (&d->desc);

  tss = (struct tss *)base;
  printf ("[TSS %x (%x:%x) \"%s\"]\n", selector, base, size, tss->osdep);
  printf ("EAX:%x EBX:%x ECX:%x EDX:%x ESI:%x EDI:%x EBP:%x\n",
	  tss->eax, tss->ebx, tss->ecx, tss->edx, tss->esi, tss->edi,
	  tss->ebp);
  printf ("ESP:%x EIP %x\n", tss->esp, tss->eip);
  printf ("CS:%x DS:%x ES:%x FS:%x GS:%x SS:%x\n", tss->cs, tss->ds, tss->es,
	  tss->fs, tss->gs, tss->ss);
  printf ("CR3:%x EFLAGS:%x LDT:%x\n", tss->cr3, tss->eflags, tss->ldt);
  printf ("BACKLINK:%x IOMAP:%x MARKER:%x\n", tss->backlink, tss->iomap,
	  tss->io_mask[TSS_IO_MASK_MAX-1]);
  printf ("SS0=%x SS1=%x SS2=%x ESP0=%x ESP1=%x ESP2=%x\n",
	  tss->ss0, tss->ss1, tss->ss2, tss->esp0, tss->esp1, tss->esp2);

  eflags_dump (tss->eflags);
}

void
tss_dump_backlink ()
{
  uint16_t selector;
  struct tss *tss;

  // Get current tss
  selector = tss_get_tr ();
  tss = tss_get_context (selector);
  printf ("current: %x(%s) backlink: %x\n", selector, tss->osdep, tss->backlink);

  // Dump interruptted tss.
  tss_dump (tss->backlink);
}

void
tss_dump_current ()
{
  uint16_t selector;

  // Get current tss
  selector = tss_get_tr ();
  tss_dump (selector);
}
