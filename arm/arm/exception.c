
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
#include <sys/console.h>
#include <cpu.h>

// User program can override these place holder.
__BEGIN_DECLS
void __user_irq_handler (void);
void __user_fiq_handler (void);
void __user_swi_handler (int);
void __user_exception_dabort (addr_t, uint32_t, uint32_t);
void __user_exception_iabort (addr_t, uint32_t, uint32_t);
void __user_exception_und (addr_t, uint32_t, uint32_t);
void user_irq_handler (void)
  __attribute__ ((weak, alias ("__user_irq_handler")));
void user_fiq_handler (void)
  __attribute__ ((weak, alias ("__user_fiq_handler")));
void user_swi_handler (void)
  __attribute__ ((weak, alias ("__user_swi_handler")));
void user_exception_dabort (void)
  __attribute__ ((weak, alias ("__user_exception_dabort")));
void user_exception_iabort (void)
  __attribute__ ((weak, alias ("__user_exception_iabort")));
void user_exception_und (void)
  __attribute__ ((weak, alias ("__user_exception_und")));
__END_DECLS

void
__user_irq_handler ()
{

  iprintf ("irq.\n");
  while (/*CONSTCOND*/1)
    ;
}

void
__user_fiq_handler ()
{

  iprintf ("fiq.\n");
  while (/*CONSTCOND*/1)
    ;
}

void
__user_swi_handler (int cause)
{

  iprintf ("SWI %d\n", cause);
}

void
__user_exception_dabort (addr_t addr, uint32_t spsr, uint32_t sp)
{

  addr -= 8;
  iprintf ("*** Data Abort. addr=%x inst=%x sp=%x ",
	   addr, *(uint32_t *)addr, sp);
  cpu_psr_dump (spsr);

  uint32_t *r = (uint32_t *)(sp - sizeof (uint32_t) * 12);
  int i;
  for (i = 0; i < 16; i++)
    iprintf ("r%d: %x\n", i, r[i]);

  assert (0);

  //  *(uint32_t *)addr = 0xe1a00000; // replace to nop (mov r0, r0).
  while (/*CONSTCOND*/1)
    ;
  //NOTREACHED
}

void
__user_exception_iabort (addr_t addr, uint32_t spsr, uint32_t sp)
{

  addr -= 4;
  iprintf ("*** Prefetch Abort. addr=%x sp=%x ", addr, sp);
  cpu_psr_dump (spsr);

  uint32_t *r = (uint32_t *)(sp - sizeof (uint32_t) * 12);
  int i;
  for (i = 0; i < 16; i++)
    iprintf ("r%d: %x\n", i, r[i]);

  assert (0);

  while (/*CONSTCOND*/1)
    ;
  //NOTREACHED
}

void
__user_exception_und (addr_t addr, uint32_t spsr __attribute__((unused)),
		      uint32_t sp)
{

  addr -= 4;
  iprintf ("*** Undefined Instruction addr=%x inst=%x sp=%x\n ",
	   addr, *(uint32_t *)addr, sp);

  //  *(uint32_t *)addr = 0xe1a00000; // replace to nop (mov r0, r0).
  //NOTREACHED
}
