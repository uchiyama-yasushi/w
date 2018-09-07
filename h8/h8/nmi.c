
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

// NMI handler for rommonitor. called from nmi@nmi_monitor.S
#include <sys/system.h>
#include <sys/console.h>

__BEGIN_DECLS
void nmi_dump_info (void);
__END_DECLS

void
nmi_dump_info ()
{
  extern char stack_start[];
  extern uint32_t *nmi_sp;

  reg_t *regs = (reg_t *)(stack_start - 32);
  reg_t ccr_pc;
  int i;

  iprintf ("\n");
  for (i = 0; i < 8; i++)
    {
      iprintf ("er%d : %lx\n", 7-i, regs[i]);
    }
  ccr_pc = *(reg_t *)regs[0];
  iprintf ("PC: %lx\n", ccr_pc & 0x00ffffff);
  iprintf ("IUHUNZVC\n");
  ibitdisp8 ((ccr_pc >> 24) & 0xff);

  *nmi_sp &= ~0x10000000;	// clear User-bit. see assert_subr()@assert.c
}
