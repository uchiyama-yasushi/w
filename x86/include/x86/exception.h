
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

#ifndef _X86_EXCEPTION_H_
#define	_X86_EXCEPTION_H_

// for 8,10,11,12,13,17
// External Event.
#define	EXCEPTION_ERROR_CODE_EXT		0x0001
#define	EXCEPTION_ERROR_CODE_IDT		0x0002
// GDT/IDT (set if LDT)
#define	EXCEPTION_ERROR_CODE_TI			0x0004
#define	EXCEPTION_ERROR_CODE_SELECTOR_MASK	0xfff8
#define	EXCEPTION_ERROR_CODE_SELECTOR_SHIFT	3
// for 14 (page fault)
//notyet

struct exception_frame
{
  uint32_t number;
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  uint32_t error_code;
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
};

typedef void (exception_handler_t) (struct exception_frame *)
  __attribute__ ((regparm (1)));

__BEGIN_DECLS
void x86_exception_init (void); // x86 exception.
void exception_init (void);	// + board specific exception.(interrupt)

// Fault handler.
void task_exception_entry (void);
void task_exception (int32_t) __attribute__((regparm (1)));
__END_DECLS
#endif
