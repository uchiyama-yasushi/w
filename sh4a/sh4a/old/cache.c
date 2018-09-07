
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
#include <cpu.h>
#include <cache.h>
#include <sys/console.h>

void
sh7785_cache_enable ()
{
  // CCR modifications must only be made by program in the P2(uncached) area.
  cpu_run_P2 ();

  // Reset cache. invalidate all. CACHED DATA IS NOT WRITE-BACKED.
   *SH7785_CCR |= (CCR_ICI | CCR_OCI);

  // I/D-cache 4way, use I-cache way-prediction.
  *SH7785_RAMCR = 0;

  // Enable I/D cache
  *SH7785_CCR = CCR_ICE | CCR_OCE;
#ifndef SH4A_EXT_ADDR32	// SH4A_EXT_ADDR32 uses PMB entry WT,C bit.
  // P0,U0,P3 write-back, P1 write-back.
  *SH7785_CCR |= CCR_CB;
#endif
  // After CCR has been updated, execute the ICBI instruction for any address.
  CPU_SYNC_RESOURCE ();
  // Return to P1(cacheable) again.
  cpu_run_P1 ();
}

void
sh7785_cache_disable ()
{
  // CCR modifications must only be made by program in the P2(uncached) area.
  cpu_run_P2 ();

  // Before invalidate, write-back all.
  sh7785_dcache_wbinv_all ();

  // Reset cache. invalidate all.
   *SH7785_CCR |= (CCR_ICI | CCR_OCI);

  // Disable I/D cache
   *SH7785_CCR &= ~(CCR_ICE | CCR_OCE);

  // After CCR has been updated, execute the ICBI instruction for any address.
  CPU_SYNC_RESOURCE ();
  // Return to P1(cacheable) again.
  cpu_run_P1 ();
}

void
sh7785_icache_sync_all ()
{
  // Memory-mapped cache array must be accessed in the P2 area program.
  cpu_run_P2 ();
  int entry;
  uint32_t addr;

  // D-cache writeback invalidate.
  sh7785_dcache_wbinv_all ();	// Index ops.

  // I-cache invalidate
  for (entry = 0; entry < SH7785_CACHE_ENTRY; entry++)
    {
      addr = SH7785_CCIA | (entry << CCDA_ENTRY_SHIFT);
      // non-associate. clear valid bit.
      *(volatile uint32_t *)(addr | 0) &= ~CCIA_V;
      *(volatile uint32_t *)(addr | (1 << CCIA_WAY_SHIFT)) &= ~CCIA_V;
      *(volatile uint32_t *)(addr | (2 << CCIA_WAY_SHIFT)) &= ~CCIA_V;
      *(volatile uint32_t *)(addr | (3 << CCIA_WAY_SHIFT)) &= ~CCIA_V;
    }
  CPU_SYNC_RESOURCE ();
  cpu_run_P1 ();
}

void
sh7785_icache_sync_range (vaddr_t va, vsize_t sz)
{
  // round/truncate with cache line.
  vsize_t eva = ROUND_CACHELINE_32 (va + sz);
  va = TRUNC_CACHELINE_32 (va);

  sh7785_dcache_wbinv_range (va, sz);

  while (va < eva)
    {
      // Hit invalidate. this may occur TLB miss.
      __asm volatile ("icbi @%0" :: "r"(va));
      va += SH7785_CACHE_LINESZ;	// next cache line.
    }
}

void
sh7785_icache_sync_range_index (vaddr_t va, vsize_t sz)
{
  cpu_run_P2 ();
  vsize_t eva = ROUND_CACHELINE_32 (va + sz);
  va = TRUNC_CACHELINE_32 (va);

  if (sz > SH7785_ICACHE_SIZE)
    {
      sh7785_icache_sync_all ();
      return;
    }

  sh7785_dcache_wbinv_range_index (va, sz);

  while (va < eva)
    {
      uint32_t addr = SH7785_CCIA | (va & (CCIA_ENTRY_MASK << CCIA_ENTRY_SHIFT));
      *(volatile uint32_t *)(addr | 0) &= ~CCIA_V;
      *(volatile uint32_t *)(addr | (1 << CCIA_WAY_SHIFT)) &= ~CCIA_V;
      *(volatile uint32_t *)(addr | (2 << CCIA_WAY_SHIFT)) &= ~CCIA_V;
      *(volatile uint32_t *)(addr | (3 << CCIA_WAY_SHIFT)) &= ~CCIA_V;
      va += SH7785_CACHE_LINESZ;
    }
  CPU_SYNC_RESOURCE ();
  cpu_run_P1 ();
}

void
sh7785_dcache_wbinv_all ()
{
  cpu_run_P2 ();
  int entry;
  uint32_t addr;

  // D-cache writeback invalidate.
  for (entry = 0; entry < SH7785_CACHE_ENTRY; entry++)
    {
      // non-associate. clear valid, dirty bit.
      addr = SH7785_CCDA | (entry << CCDA_ENTRY_SHIFT);
      // flush all way.
      *(volatile uint32_t *)(addr | 0) &= ~(CCDA_U | CCDA_V);
      *(volatile uint32_t *)(addr | (1 << CCDA_WAY_SHIFT)) &= ~(CCDA_U | CCDA_V);
      *(volatile uint32_t *)(addr | (2 << CCDA_WAY_SHIFT)) &= ~(CCDA_U | CCDA_V);
      *(volatile uint32_t *)(addr | (3 << CCDA_WAY_SHIFT)) &= ~(CCDA_U | CCDA_V);
    }
  CPU_SYNC_RESOURCE ();
  cpu_run_P1 ();
}

void
sh7785_dcache_wbinv_range (vaddr_t va, vsize_t sz)
{
  if (sz > SH7785_DCACHE_SIZE)
    {
      sh7785_dcache_wbinv_all ();	// Index ops.
      return;
    }

  vsize_t eva = ROUND_CACHELINE_32 (va + sz);
  va = TRUNC_CACHELINE_32 (va);

  while (va < eva)
    {
      // Hit write-back-invalidate. this may occur TLB miss.
      __asm volatile ("ocbp @%0" :: "r"(va));
      va += SH7785_CACHE_LINESZ;	// next cache line.
    }
  __asm volatile ("synco");
}

void
sh7785_dcache_inv_range (vaddr_t va, vsize_t sz)
{
  vsize_t eva = ROUND_CACHELINE_32 (va + sz);
  va = TRUNC_CACHELINE_32 (va);

  while (va < eva)
    {
      // Hit invalidate. this may occur TLB miss.
      __asm volatile ("ocbi @%0" :: "r"(va));
      va += SH7785_CACHE_LINESZ;	// next cache line.
    }
}

void
sh7785_dcache_wb_range (vaddr_t va, vsize_t sz)
{
  vsize_t eva = ROUND_CACHELINE_32 (va + sz);
  va = TRUNC_CACHELINE_32 (va);

  while (va < eva)
    {
      // Hit write-back. this may occur TLB miss.
      __asm volatile ("ocbwb @%0" :: "r"(va));
      va += SH7785_CACHE_LINESZ;	// next cache line.
    }
  __asm volatile ("synco");
}

void
sh7785_dcache_wbinv_range_index (vaddr_t va, vsize_t sz)
{
  cpu_run_P2 ();
  vsize_t eva = ROUND_CACHELINE_32 (va + sz);
  va = TRUNC_CACHELINE_32 (va);

  if (sz > SH7785_DCACHE_SIZE)
    {
      sh7785_dcache_wbinv_all ();	// Index ops.
      return;
    }

  while (va < eva)
    {
      uint32_t addr = SH7785_CCDA | (va & (CCIA_ENTRY_MASK << CCIA_ENTRY_SHIFT));
      *(volatile uint32_t *)(addr | 0) &= ~(CCDA_U | CCDA_V);
      *(volatile uint32_t *)(addr | (1 << CCIA_WAY_SHIFT)) &= ~(CCDA_U | CCDA_V);
      *(volatile uint32_t *)(addr | (2 << CCIA_WAY_SHIFT)) &= ~(CCDA_U | CCDA_V);
      *(volatile uint32_t *)(addr | (3 << CCIA_WAY_SHIFT)) &= ~(CCDA_U | CCDA_V);
      va += SH7785_CACHE_LINESZ;
    }
  CPU_SYNC_RESOURCE ();
  cpu_run_P1 ();
}

#ifdef DEBUG
#include <sys/console.h>
void
sh7785_dcache_array_dump (vaddr_t va, vsize_t sz)
{
  cpu_run_P2 ();
  vsize_t eva = ROUND_CACHELINE_32 (va + sz);
  va = TRUNC_CACHELINE_32 (va);

  size_t j, k;

  while (va < eva)
    {
      uint32_t entry = va & (CCDD_ENTRY_MASK << CCDD_ENTRY_SHIFT);
      uint32_t d0 = SH7785_CCDD | entry;
      uint32_t a0 = SH7785_CCDA | entry;
      iprintf ("[Entry %x](%x)----------------------------------------------\n",
	       (va >> CCDD_ENTRY_SHIFT) & CCDD_ENTRY_MASK, entry);
      for (k = 0; k < SH7785_CACHE_WAY; k++)
	{
	  uint32_t way = k << CCDD_WAY_SHIFT;
	  uint32_t d1 = d0 | way;
	  uint32_t bit = *(volatile uint32_t *)(a0 | way);
	  iprintf ("way %d tag=%x (%c|%c)\n", k,
		   bit & (CCDA_TAG_MASK << CCDA_TAG_SHIFT),
		   bit & CCDA_U ? 'D' : '-',
		   bit & CCDA_V ? 'V' : '-');

	  // Dump cache line.
	  for (j = 0; j < SH7785_CACHE_LINESZ / sizeof (int32_t); j++)
	    {
	      uint32_t line = j << CCDD_LINE_SHIFT;
	      iprintf ("%x ",  *(volatile uint32_t *)(d1 | line));
	    }
	  iprintf ("\n");
	}
      va += SH7785_CACHE_LINESZ;
    }

  CPU_SYNC_RESOURCE ();
  cpu_run_P1 ();
}


#endif
