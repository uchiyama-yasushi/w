
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

// Memory Management Unit

#include <sys/system.h>
#include <cpu.h>
#include <mmu.h>
#ifdef DEBUG
#include <sys/console.h>
STATIC void tlb_entry_print (uint32_t, uint32_t, uint32_t, int);
#endif

void
sh4a_mmu_start ()
{
  uint32_t r;
  // Set ASID
  *SH4A_PTEH = 0;

  // Invalidate all entries. and start Address translation.
  r  = MMUCR_TI | MMUCR_AT;
#ifdef SH4A_EXT_MMU
  r |= MMUCR_ME;
#endif

  *SH4A_MMUCR = r;

  CPU_SYNC_RESOURCE ();
}

void
sh4a_mmu_asid_set (uint32_t acid)
{

  *SH4A_PTEH = acid & PTEH_ASID;
}

uint32_t
sh4a_mmu_asid_get ()
{

  return *SH4A_PTEH & PTEH_ASID;
}

void
sh4a_tlb_counter_set (uint32_t n)
{

  *SH4A_MMUCR &= ~(MMUCR_URC_MASK << MMUCR_URC_SHIFT);
  *SH4A_MMUCR |= (n << MMUCR_URC_SHIFT);
}

uint32_t
sh4a_tlb_counter_get ()
{

  return (*SH4A_MMUCR >> MMUCR_URC_SHIFT) & MMUCR_URC_MASK;
}

void
sh4a_mmu_wired_set (int n)
{
  uint32_t limit = (SH4A_UTLB_ENTRY - n) & MMUCR_URB_MASK;

  // If current counter is wired area, reset.
  if (sh4a_tlb_counter_get () >= limit)
    sh4a_tlb_counter_set (0);
  // Set coutner limit.
  *SH4A_MMUCR &= ~(MMUCR_URB_MASK << MMUCR_URB_SHIFT);
  *SH4A_MMUCR |= (limit << MMUCR_URB_SHIFT);

  iprintf ("wired entry: %d-%d\n", limit, SH4A_UTLB_ENTRY - 1);
}

uint32_t
sh4a_mmu_wired_get ()
{
  // return # of wired entry. if URC == URB, URC is setted 0.
  uint32_t urb = (*SH4A_MMUCR >> MMUCR_URB_SHIFT) & MMUCR_URB_MASK;

  return urb == 0 ? 0 : SH4A_UTLB_ENTRY - urb;	// urb == 0: no wired entry.
}

void
sh4a_tlb_clear_all ()
{
  uint32_t entry, e;

  for (entry = 0; entry < SH4A_ITLB_ENTRY; entry++)
    {
      e = entry << ITLB_AA_E_SHIFT;
      *(volatile uint32_t *)(SH4A_ITLB_AA | e) = 0;
      *(volatile uint32_t *)(SH4A_ITLB_DA1 | e) = 0;
#ifdef SH4A_EXT_MMU
      *(volatile uint32_t *)(SH4A_ITLB_DA2 | e) = 0;
#endif
    }
  for (entry = 0; entry < SH4A_UTLB_ENTRY; entry++)
    {
      e = entry << UTLB_AA_E_SHIFT;
      *(volatile uint32_t *)(SH4A_UTLB_AA | e) = 0;
      *(volatile uint32_t *)(SH4A_UTLB_DA1 | e) = 0;
#ifdef SH4A_EXT_MMU
      *(volatile uint32_t *)(SH4A_UTLB_DA2 | e) = 0;
#endif
    }
}

void
sh4a_tlb_wired_entry (struct tlb_entry *e)
{
  uint32_t pteh, ptel, ptea;
#ifdef DEBUG
  uint32_t pgsz_table[] = { 0x400, 0x1000, 0x1000, 0x100000, 0x2000, 0x40000,
			    0x400000, 0x4000000 };
  uint32_t pgsz_mask = pgsz_table[e->size] -1;
  assert (!(e->vpn & pgsz_mask) && !(e->ppn & pgsz_mask));
#endif
  // Set Physical addr - Virtual addr.
  ptel = e->ppn & PTEL_PPN;
  pteh = e->vpn & PTEH_VPN;
  ptea = 0;

  // Shared page don't bother ASID.
  if (e->shared)
    ptel |=  PTEL_SH;
  else
    pteh |= e->asid;
  // Setup Protect, Cache, Pagesize.
  sh4a_tlb_entry_pagesize (e->size, &ptel, &ptea);
  sh4a_tlb_entry_protect (e->protect, &ptel, &ptea);
  sh4a_tlb_entry_cache (e->cache, &ptel, &ptea);

  // Enable this entry.
  ptel |= (PTEL_V | PTEL_D);
  // Prepare new wired entry.
  uint32_t n = sh4a_mmu_wired_get () + 1;

  // Set URC for LDTLB
  sh4a_tlb_counter_set (SH4A_UTLB_ENTRY - n);
  *SH4A_PTEH = pteh;
  *SH4A_PTEL = ptel;
#ifdef SH4A_EXT_MMU
  *SH4A_PTEA = ptea;
#endif
  __asm volatile ("ldtlb");
  // Make this wired entry.
  sh4a_mmu_wired_set (n);
  CPU_SYNC_RESOURCE ();
#ifdef DEBUG
  // Valid bit is maped to both address array and data array, but
  // tlb_entry_print check address array only. XXX
  tlb_entry_print (pteh | UTLB_AA_V, ptel, ptea, 0);
#endif
}

// Invalidate TLB entry by address.
void
sh4a_tlb_invalidate_addr (uint32_t asid, vaddr_t va)
{
  cpu_status_t s = intr_suspend();
  // Save current ASID
  uint32_t oasid = sh4a_mmu_asid_get ();
  bool save_asid = asid != oasid;

  // Set ASID for associative write
  if (save_asid)
    sh4a_mmu_asid_set (asid);

  // Associative write UTLB. ITLB is also invalidated.
  // SH4A can access by a program in the P1/P2. no need to change P2.
  *(volatile uint32_t *)(SH4A_UTLB_AA | UTLB_AA_A) = va & PTEH_VPN;

  // Restore ASID
  if (save_asid)
    sh4a_mmu_asid_set (oasid);
  intr_resume (s);
}


// Invalidate TLB entry by ASID.
void
sh4a_tlb_invalidate_asid (uint32_t asid)
{
  cpu_status_t s = intr_suspend ();	// don't thread switch here.
  volatile uint32_t *a;
  uint32_t entry;
  // I assume asid is not 0. so wired entry is not matched.

  for (entry = 0; entry < SH4A_UTLB_ENTRY; entry++)
    {
      a = (volatile uint32_t *)(SH4A_UTLB_AA | (entry << UTLB_AA_E_SHIFT));
      if ((*a & UTLB_AA_ASID) == (uint32_t)asid)
	*a = 0;
    }

  for (entry = 0; entry < SH4A_ITLB_ENTRY; entry++)
    {
      a = (volatile uint32_t *)(SH4A_ITLB_AA | (entry << ITLB_AA_E_SHIFT));
      if ((*a & ITLB_AA_ASID) == (uint32_t)asid)
	*a = 0;	// Drop valid bit.
    }

  intr_resume (s);
}

// Invalidate TLB entry exclude wired entry.
void
sh4a_tlb_invalidate_all ()
{
  cpu_status_t s = intr_suspend ();
  volatile uint32_t *a;
  uint32_t limit, entry;

  limit = SH4A_UTLB_ENTRY - sh4a_mmu_wired_get ();
  for (entry = 0; entry < limit; entry++)
    {
      *(volatile uint32_t *)(SH4A_UTLB_AA | (entry << UTLB_AA_E_SHIFT)) = 0;
    }

  for (entry = 0; entry < SH4A_ITLB_ENTRY; entry++)
    {
      a = (volatile uint32_t *)(SH4A_ITLB_AA | (entry << ITLB_AA_E_SHIFT));
      *a = 0;
    }

  intr_resume (s);
}

void
sh4a_tlb_entry_pagesize (int szidx,  uint32_t *ptel __attribute__((unused)),
			 uint32_t *ptea __attribute__((unused)))
{
#ifdef SH4A_EXT_MMU
  uint32_t pgsz_bits[] = { PTEA_ESZ_1K, PTEA_ESZ_4K, PTEA_ESZ_64K,
			   PTEA_ESZ_1M, PTEA_ESZ_8K, PTEA_ESZ_256K,
			   PTEA_ESZ_4M, PTEA_ESZ_64M };
  *ptea |= pgsz_bits[szidx];
#else
  uint32_t pgsz_bits[] = { PTEL_SZ_1K, PTEL_SZ_4K, PTEL_SZ_64K, PTEL_SZ_1M };
  *ptel |= pgsz_bits[szidx];
#endif
}

void
sh4a_tlb_entry_protect (int pridx,  uint32_t *ptel __attribute__((unused)),
			uint32_t *ptea __attribute__((unused)))
{
#ifdef SH4A_EXT_MMU
  uint32_t pr_bits [] = { PTEA_EPR_P_R | PTEA_EPR_P_X,
			  PTEA_EPR_P_R | PTEA_EPR_P_W | PTEA_EPR_P_X,
			  PTEA_EPR_P_R | PTEA_EPR_P_W | PTEA_EPR_P_X |
			  PTEA_EPR_U_R | PTEA_EPR_U_X,
			  PTEA_EPR_P_R | PTEA_EPR_P_W | PTEA_EPR_P_X |
			  PTEA_EPR_U_R | PTEA_EPR_U_W | PTEA_EPR_U_X };
  *ptea |= pr_bits[pridx];
#else
  uint32_t pr_bits[] = { PTEL_PR_P_RDONLY, PTEL_PR_P_RW, PTEL_PR_U_RDONLY,
			 PTEL_PR_U_RW };

  *ptel |= pr_bits[pridx];
#endif
}

void
sh4a_tlb_entry_cache (int cidx,  uint32_t *ptel,
		      uint32_t *ptea __attribute__((unused)))
{
  uint32_t cache_bits[] = { 0, PTEL_C, PTEL_C | PTEL_WT };

  *ptel |= cache_bits[cidx];
}


uint32_t
sh4a_mmu_encode_swbits (enum mmu_page_size sz,	// 8types. 3bit
			enum mmu_protect pr,	// 4types. 2bit
			uint32_t opt __attribute__((unused)))
{
  uint32_t pte = 0;

  pte |= (sz & 1) << 4;
  pte |= (sz & 2) << 6;
#ifdef SH4A_EXT_MMU
  pte |= (sz & 4) << 6;
#endif
  pte |= pr << 5;

  return pte;
}

// Load new TLB entry.
void
sh4a_tlb_update(uint32_t asid, vaddr_t va, uint32_t pte)
{
  cpu_status_t s = intr_suspend ();
  // Save current ASID
  uint32_t oasid = sh4a_mmu_asid_get ();
  bool save_asid = asid != oasid;
  if (save_asid)
    sh4a_mmu_asid_set (asid);

  pte = pte; //XXX GCC WARINIG


  // Invalidate old entry.
  sh4a_tlb_invalidate_addr (asid, va);

  // Load new entry.
  uint32_t vpn = va & PTEH_VPN;
  uint32_t pteh = (vpn | asid) & PTEH_VALID_BITS;
  *SH4A_PTEH = pteh;
#ifdef SH4A_EXT_MMU
  uint32_t ptel, ptea;
  pte = pte; ptel = 0; ptea=0;
  //  sh4a_mmu_decode_swbits (pte, &ptel, &ptea);
  *SH4A_PTEL = ptel;
  *SH4A_PTEA = ptea;
#else
  uint32_t ptel = pte & PTEL_VALID_BITS;
  //  sh4a_mmu_decode_swbits (pte, &ptel, 0);
  *SH4A_PTEL = ptel;
#endif
  __asm volatile ("ldtlb");
  CPU_SYNC_RESOURCE ();

  // Restore ASID
  if (save_asid)
    sh4a_mmu_asid_set (oasid);

  intr_resume (s);
}

#ifdef DEBUG
void
sh4a_mmu_dump ()
{

  iprintf ("MMUCR: %x ", *SH4A_MMUCR);
  iprintf ("PTEH: %x ", *SH4A_PTEH);
  iprintf ("PTEL: %x ", *SH4A_PTEL);
  iprintf ("TTB: %x ", *SH4A_TTB);
  iprintf ("TEA: %x ", *SH4A_TEA);
#ifdef SH4A_EXT_MMU
  iprintf ("PTEA: %x", *SH4A_TEA);
#endif
  iprintf ("\n");
  sh4a_mmu_itlb_dump ();
  sh4a_mmu_utlb_dump ();
}

void
sh4a_mmu_itlb_dump ()
{
  uint32_t entry, e, aa, da1, da2;

  iprintf ("--------------------ITLB--------------------\n");
  for (entry = 0; entry < SH4A_ITLB_ENTRY; entry++)
    {
      e = entry << ITLB_AA_E_SHIFT;
      aa = *(volatile uint32_t *)(SH4A_ITLB_AA | e);
      da1 = *(volatile uint32_t *)(SH4A_ITLB_DA1 | e);
#ifdef SH4A_EXT_MMU
      da2 = *(volatile uint32_t *)(SH4A_ITLB_DA2 | e);
#else
      da2 = 0;
#endif
      tlb_entry_print (aa, da1, da2, entry);
    }
}

void
sh4a_mmu_utlb_dump ()
{
  uint32_t entry, e, aa, da1, da2;

  iprintf ("--------------------UTLB--------------------\n");
  for (entry = 0; entry < SH4A_UTLB_ENTRY; entry++)
    {
      e = entry << UTLB_AA_E_SHIFT;
      aa = *(volatile uint32_t *)(SH4A_UTLB_AA | e);
      da1 = *(volatile uint32_t *)(SH4A_UTLB_DA1 | e);
#ifdef SH4A_EXT_MMU
      da2 = *(volatile uint32_t *)(SH4A_UTLB_DA2 | e);
#else
      da2 = 0;
#endif
      tlb_entry_print (aa, da1, da2, entry);
    }
}

void
tlb_entry_print (uint32_t aa, uint32_t da1, uint32_t da2, int n)
{
  uint32_t vpn = aa & UTLB_AA_VPN;
  bool pmb = (vpn & 0xc0000000) == 0x80000000;
  int sz;
  int valid = aa & UTLB_AA_V;

  da2 = da2; // XXX GCC Warning

  if (!valid)
    return;

  iprintf ("(%c|%c) ", aa & UTLB_AA_D ? 'D' : '-', valid ? 'V' : '-');
  const char *pmb_pgsz[] = { " 16M", " 64M", "128M", "512M" };
#ifdef SH4A_EXT_MMU
  const char *pgsz[] = { "  1K", "  4K", "  8K", "-XX-",
			 " 64K", "256K", "-XX-", "  1M",
			 "  4M", "-XX-", "-XX-", "-XX-",
			 " 64M", "-XX-", "-XX-", "-XX-" };
  if (pmb)
    sz = ((da1 & PMB_DA_SZ0) >> 4) | ((da1 & PMB_DA_SZ1) >> 6);
  else
    sz = (da2 >> 4) & 0xf;
  if (!pmb)
    {
      iprintf ("[%c%c%c][%c%c%c] ",
	       da2 & UTLB_DA2_EPR_P_R ? 'r' : '-',
	       da2 & UTLB_DA2_EPR_P_W ? 'w' : '-',
	       da2 & UTLB_DA2_EPR_P_X ? 'x' : '-',
	       da2 & UTLB_DA2_EPR_U_R ? 'r' : '-',
	       da2 & UTLB_DA2_EPR_U_W ? 'w' : '-',
	       da2 & UTLB_DA2_EPR_U_X ? 'x' : '-');
    }
#else
  const char *pgsz[] = { "  1K", "  4K", " 64K", "  1M" };
  sz = ((da1 & UTLB_SZ0) >> 4) | ((da1 & UTLB_SZ1) >> 6);

  if (!pmb)
    {
      int pr = (da1 >> 5) & 0x3;
      const char *pr_pat[] = { "[r-][--]",
			       "[rw][--]",
			       "[rw][r-]",
			       "[rw][rw]" };
      iprintf ("%s ", pr_pat[pr]);
    }
#endif
  iprintf ("%s ", pmb ? pmb_pgsz[sz] : pgsz[sz]);

#ifdef SH4A_EXT_ADDR32
  iprintf ("%s ", da1 & UTLB_DA1_UB ? "UB" : "--");
#endif
  if (da1 & UTLB_DA1_C)
    {
      iprintf ("W%c ", da1 & UTLB_DA1_WT ? 'T' : 'B');
    }
  else
    {
      iprintf ("-- ");
    }
  bool shared_page = da1 & UTLB_DA1_SH;
  if (!pmb)
    iprintf ("%s ", shared_page ? "SH" : "--");
  iprintf ("VPN:%x PPN:%x ", aa & UTLB_AA_VPN, da1 & UTLB_DA1_PPN);
  if (pmb)
    iprintf ("PMB ");
  else if (!shared_page)
    iprintf ("ASID:%x ", aa & UTLB_AA_ASID);

  iprintf ("[%d]\n", n);
}
#endif	// DEBUG
