
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
#include <mmu.h>

uint32_t cpu_input_clock;
int cpu_mode;
int cpu_paddr;

void
cpu_init (uint32_t input_clock/*Hz*/, int mode)
{
  cpu_input_clock = input_clock;
  cpu_mode = mode;

#ifdef SH4A_EXT_ADDR32
  *PASCR = PASCR_SE;
  cpu_paddr = 32;
#else
  // CPU doesn't wait for the end of writing bus access and starts
  // next bus access
  *PASCR = 0;
  cpu_paddr = 29;
#endif

  // Don't re-fetch all.
  *IRMCR = IRMCR_R2 | IRMCR_R1 | IRMCR_LT | IRMCR_MT | IRMCR_MC;
}

void
cpu_info ()
{

  iprintf ("CPU mode %d, %dbit address mode, input clock %dMHz\n",
	   cpu_mode, cpu_paddr, cpu_input_clock / 1000000);
}


#ifdef DEBUG
void
cpu_dump_sr (uint32_t r)
{

  iprintf ("%s-mode, bank %d, Exception %sabled, FPU %sabled, IMASK=0x%x\n",
	   r & SR_MD ? "Privilege" : "User", r & SR_RB ? 1 : 0,
	   r & SR_BL ? "dis" : "en",
	   r & SR_FD ? "dis" : "en",
	   (r >> SR_IMASK_SHIFT) & SR_IMASK_MASK);
}
#endif
