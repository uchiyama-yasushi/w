
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
#include "gdt.h"

STATIC struct gate_descriptor idt[IDT_MAX];
STATIC void null_gate (void) __attribute ((noreturn));
STATIC bool idt_initialized = FALSE;

struct lidt_arg protectmode_lidt_arg =
  {
    sizeof idt - 1, (uint32_t)idt
  };

void
null_gate ()
{

  iprintf ("null gate. stop.\n");
  while (/*CONSTCOND*/1)
    ;
  /* NOTREACHED */
}

void
idt_install (const struct vector_config *conf)
{
  int i;

  // Install place holder 1st.
  if (!idt_initialized)
    {
      for (i = 0; i < IDT_MAX; i++)
	gate_interrupt (idt + i, GDT_CODE32, null_gate);
      idt_initialized = TRUE;
    }

  // Install actual gate.
  for (; conf->number != 0x100; conf++)
    {
      gate (idt + conf->number, conf->type, conf->selector, conf->handler,
	    conf->nparam);
    }

  __asm volatile ("lidt %0" :: "m"(protectmode_lidt_arg));
  // check.
  //struct lidt_arg sidt;
  //  __asm volatile ("sidt %0" : "=m"(sidt):);
  //  printf ("IDT: base=0x%x limit=0x%x\n", sidt.base, sidt.limit);
}

