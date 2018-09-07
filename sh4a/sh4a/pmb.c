
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

// Physical Address Space. (SH4A_EXT_ADDR32)

#ifdef SH4A_EXT_ADDR32	//32bit physical addrres mode.
#include <sys/system.h>
#include <cpu.h>
#include <mmu.h>
#ifdef DEBUG
#include <sys/console.h>
#endif

STATIC int pmb_page_size[] = { 16, 64, 128, 512 };

bool
sh4a_pmb_align_check (uint32_t aa, uint32_t da)
{
  int pgmb = pmb_page_size[_PMB_SZ_INDEX (da)];
  uint32_t mask = pgmb * 1024 * 1024 - 1;
  uint32_t vpn = aa & PMB_AA_VPN;
  uint32_t ppn = da & PMB_DA_PPN;
  bool vpn_ok = !(vpn & mask);
  bool ppn_ok = !(ppn & mask);

  iprintf ("PMB page size %dMB mask=%x VPN:%x%s PPN:%x%s\n", pgmb, mask,
	   vpn, vpn_ok ? "" : "***NG***", ppn, ppn_ok ? "" : "***NG***");

  return vpn_ok && ppn_ok;
}

void
sh4a_pmb_entry_get (int entry, uint32_t *aa, uint32_t *da)
{
  uint32_t e = entry << PMB_AA_E_SHIFT;

  *aa = *(volatile uint32_t *)(PMB_AA | e);
  *da = *(volatile uint32_t *)(PMB_DA | e);
}

void
sh4a_pmb_entry_set (int entry, uint32_t aa, uint32_t da)
{
  uint32_t e = entry << PMB_AA_E_SHIFT;

  aa &= PMB_AA_VALID_BIT;
  da &= PMB_DA_VALID_BIT;
  // Valid bit is mapped to both AA and DA.
  aa |= PMB_AA_V;
  da &= ~PMB_DA_V;
  *(volatile uint32_t *)(PMB_DA | e) = da;
  *(volatile uint32_t *)(PMB_AA | e) = aa;
}

void
sh4a_pmb_entry_dump (uint32_t aa, uint32_t da)
{

  if (!(aa & PMB_AA_V))
    return;

  iprintf ("VPN:%x (%c) ", aa & PMB_AA_VPN, aa & PMB_AA_V ? 'V' : '-');
  iprintf ("PPN:%x (%c) ", da & PMB_DA_PPN, da & PMB_DA_V ? 'V' : '-');
  iprintf ("%dMB, ", pmb_page_size[_PMB_SZ_INDEX (da)]);
  iprintf ("%sbufferd write, ", da & PMB_DA_UB ? "un" : "");
  iprintf ("%scacheable, ", da & PMB_DA_C ? "" : "un");
  iprintf ("write-%s, ", da & PMB_DA_WT ? "thru" : "back");

  iprintf ("\n");
}
#ifdef DEBUG
void
sh4a_pmb_dump ()
{
  uint32_t r;
  int entry;
  int i;

  iprintf ("--------------------PMB--------------------\n");
  r = *PASCR;
  iprintf ("paddr %dbit mode.\n", r & PASCR_SE ? 32 : 29);
  if (!(r & PASCR_SE))
    {
      // These are effectable for 29bit mode.
      iprintf ("Buffered write setting: |");
      for (i = 0; i < 8; i++)
	iprintf ("%c", r & (1 << i) ? 'x' : '-');
      iprintf ("| x:unbuffered write.\n");
    }

  for (entry = 0; entry < PMB_ENTRY; entry++)
    {
      uint32_t aa, da;
      sh4a_pmb_entry_get (entry, &aa, &da);
      sh4a_pmb_entry_dump (aa, da);
    }
}
#endif
#endif // SH4A_EXT_ADDR32
