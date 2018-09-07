
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
#include <fpool.h>
#include <pte.h>
#include <vm.h>
#include <cpu.h>
#include <mmu.h>
#include <string.h>

struct pte ptp_slot_p0[PTP_ENTRY];
struct pte ptp_slot_p3[PTP_ENTRY];

void pte_setup (bool, struct pte *, paddr_t);
void pager_tlb_update (struct pte *, vaddr_t);
void alloc_page_p3 (struct vm_page **, struct pte *);

#define	PTP_INDEX_OF(x) (((x) >> PTP_INDEX_SHIFT) & PTP_INDEX_MASK)

void
pager_load_page (vaddr_t vaddr, uint32_t spc)
{
  struct pte *ptp;
  struct vm_page *vm;

  iprintf ("%s va=%x PC=%x ", __FUNCTION__, vaddr, spc);


  if (vaddr & 0x80000000)
    {
      iprintf ("P3\n");
      ptp = &ptp_slot_p3[PTP_INDEX_OF (vaddr & 0x1fffffff)];
    }
  else
    {
      iprintf ("P0\n");
      ptp = &ptp_slot_p0[PTP_INDEX_OF (vaddr)];
    }

  if (ptp->ptel == 0)	// Page table page not allocated.
    {
      iprintf ("No page table page\n");
      alloc_page_p3 (&vm, ptp);
      iprintf ("PTP allocated pa=%x va=%x\n", vm->pa, vm->va);
    }
  else
    {
      vm = vm_lookup_by_paddr (ptp->ptel & PTEL_PPN);
      sh4a_tlb_invalidate_addr (0, vm->va);
      pager_tlb_update (ptp, vm->va);
      iprintf ("PTP found. pa=%x va=%x\n", ptp->ptel & PTEL_PPN, vm->va);
    }
  // Page table page is setuped.

  // Target page table entry.
  struct pte *pte = (struct pte *)vm_vaddr (vm) +
    ((vaddr >> PTP_OFFSET_SHIFT) & PTP_OFFSET_MASK);
  iprintf ("PTE=%x\n", pte);

  if (pte->ptel == 0)
    {
      // not mapped area.
      paddr_t pa = fpool_get_page ();
      pte_setup (FALSE, pte, pa);
      vm = vm_page_allocate ();
      vm_page_register (vm, vaddr, pa);
      iprintf ("New map pa=%x va=%x\n", pa, vaddr);
   }
  pager_tlb_update (pte, vaddr);
}

void
pte_setup (bool kernel, struct pte *pte, paddr_t pa)
{
  uint32_t ptel, ptea;

  ptel = pa & PTEL_PPN;
  ptea = 0;
#ifdef SH4A_EXT_MMU
  sh4a_tlb_entry_pagesize (SH4A_PG_8K, &ptel, &ptea);
#else
  sh4a_tlb_entry_pagesize (SH4A_PG_4K, &ptel, &ptea);
#endif

  //  sh4a_tlb_entry_protect (U_RW, &ptel, &ptea);
  if (kernel)
    sh4a_tlb_entry_protect (P_RW, &ptel, &ptea);
  else
    sh4a_tlb_entry_protect (P_RDONLY, &ptel, &ptea);
  sh4a_tlb_entry_cache (WRITEBACK, &ptel, &ptea);

  pte->ptel = ptel | PTEL_D | PTEL_V;
  pte->ptea = ptea;
}

void
pager_tlb_update (struct pte *pte, vaddr_t va)
{
  // Increment UTLB counter.
  int cnt = (sh4a_tlb_counter_get () + 1) & MMUCR_URC_MASK;
  sh4a_tlb_counter_set (cnt);

  *SH4A_PTEH = va & PTEH_VPN;
  *SH4A_PTEL = pte->ptel;
#ifdef SH4A_EXT_MMU
  *SH4A_PTEA = pte->ptea;
#endif
  __asm volatile ("ldtlb");
  CPU_SYNC_RESOURCE ();
}

void
alloc_page_p3 (struct vm_page **vmp, struct pte *ptep)
{
  struct vm_page *vm;
  struct pte *pte;
  paddr_t pa;
  vaddr_t va;
  iprintf("hello0\n");
  pa = fpool_get_page ();
  iprintf("hello1\n");
  vm = vm_page_allocate ();
  iprintf("hello2\n");
  va = vm_page_map_p3 (vm);	// mapped to P3.
  iprintf("hello3\n");
  vm_page_register (vm, va, pa);
  iprintf("hello4\n");
  // Load entry to the TLB.
  pte = &ptp_slot_p3[PTP_INDEX_OF (va & 0x1fffffff)];
  iprintf("hello5\n");
  pte_setup (TRUE, pte, pa);
  iprintf("hello6\n");
  pager_tlb_update (pte, va);
  iprintf("hello7\n");
  // Clear page table page.
  memset ((void *)va, 0, PAGE_SIZE);
  iprintf("hello8\n");
  *vmp = vm;
  *ptep = *pte;
}
