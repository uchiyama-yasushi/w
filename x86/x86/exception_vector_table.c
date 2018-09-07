
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

#include <x86/cpu.h>
#include <x86/segment.h>
#include <x86/gate.h>
#include <x86/exception.h>
#include <x86/tss.h>
#include "gdt.h"

gate_handler fault00_divided_by_zero;
gate_handler trap_fault01_debug;
gate_handler trap02_nmi;
gate_handler trap03_break_point;
gate_handler trap04_overflow;
gate_handler fault05_bound_range_exceed;
gate_handler fault06_invalid_opcode;
gate_handler fault07_extension_not_available;
gate_handler abort09_cop_segment_overrun;
gate_handler fault0b_no_segment;
gate_handler fault0d_general_protection;
gate_handler fault0e_page_fault;
gate_handler fault10_coprocessor_error;
gate_handler fault11_alignment;
gate_handler abort12_machine_check;

const struct vector_config x86_vector_table[] =
  {
    { 0x00, SYS_INTRGATE, 0, GDT_CODE32, fault00_divided_by_zero },
    { 0x01, SYS_INTRGATE, 0, GDT_CODE32, trap_fault01_debug},
    { 0x02, SYS_INTRGATE, 0, GDT_CODE32, trap02_nmi },
    { 0x03, SYS_INTRGATE, 0, GDT_CODE32, trap03_break_point },
    { 0x04, SYS_INTRGATE, 0, GDT_CODE32, trap04_overflow },
    { 0x05, SYS_INTRGATE, 0, GDT_CODE32, fault05_bound_range_exceed },
    { 0x06, SYS_INTRGATE, 0, GDT_CODE32, fault06_invalid_opcode },
    { 0x07, SYS_INTRGATE, 0, GDT_CODE32, fault07_extension_not_available },
    { 0x08, SYS_TASKGATE, 0, GDT_TSS_DOUBLE_FAULT, 0 }, // double fault
    { 0x09, SYS_INTRGATE, 0, GDT_CODE32, abort09_cop_segment_overrun },
    { 0x0a, SYS_TASKGATE, 0, GDT_TSS_INVALID_TSS, 0 }, //invalid_tss
    { 0x0b, SYS_INTRGATE, 0, GDT_CODE32, fault0b_no_segment },
    { 0x0c, SYS_TASKGATE, 0, GDT_TSS_STACK_OVERRUN, 0 },//stack overrun
    { 0x0d, SYS_INTRGATE, 0, GDT_CODE32, fault0d_general_protection },
    { 0x0e, SYS_INTRGATE, 0, GDT_CODE32, fault0e_page_fault },
    { 0x10, SYS_INTRGATE, 0, GDT_CODE32, fault10_coprocessor_error },
    { 0x11, SYS_INTRGATE, 0, GDT_CODE32, fault11_alignment },
    { 0x12, SYS_INTRGATE, 0, GDT_CODE32, abort12_machine_check },
    { 0x100, 0, 0, 0, 0 }, // Terminator.
  };

exception_handler_t exception_number_print_with_arg;
exception_handler_t exception_number_print;

void
x86_exception_init ()
{

  idt_install (x86_vector_table);
}

void __attribute__ ((regparm (1)))
exception_number_print_with_arg (struct exception_frame *r)
{

  printf ("<%d> %x\n", r->number, r->error_code);
  printf ("EAX:%x EBX:%x ECX:%x EDX:%x ESI:%x EDI:%x EBP:%x ESP:%x\n",
	  r->eax, r->ebx, r->ecx, r->edx, r->esi, r->edi, r->ebp, r->esp);
  printf ("CS=%x: EIP=%x EFLAGS=%x\n", r->cs, r->eip, r->eflags);
  eflags_dump (r->eflags);
  tss_call (GDT_TSS_TASK0);
  while (/*CONSTCOND*/1)
      ;
  // NOTREACHED
}

void  __attribute__ ((regparm (1)))
exception_number_print (struct exception_frame *r)
{

  printf ("[%d]\n", r->number);
  printf ("EAX:%x EBX:%x ECX:%x EDX:%x ESI:%x EDI:%x EBP:%x ESP:%x\n",
	  r->eax, r->ebx, r->ecx, r->edx, r->esi, r->edi, r->ebp, r->esp);
  printf ("CS=%x: EIP=%x EFLAGS=%x\n", r->eip, r->error_code, r->cs);
  eflags_dump (r->cs);

  while (/*CONSTCOND*/1)
      ;
  // NOTREACHED
}

void __attribute__ ((regparm (1)))
task_exception (int32_t error_code)
{

  printf ("%s: error code=0x%x\n", __FUNCTION__, error_code);

  tss_dump_backlink ();
  while (/*CONSTCOND*/1)
    ;
}
