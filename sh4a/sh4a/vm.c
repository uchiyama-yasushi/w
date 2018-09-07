
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

// Virtual Memory.
#include <sys/system.h>
#include <sys/console.h>
#include <pte.h>
#include <vm.h>

struct vm_page __vm_pool[1024]; //XXX for now.
int __vm_pool_index;
vaddr_t vm_base = 0xc0000000;

struct vm_page *
vm_page_allocate ()
{
  struct vm_page *vm = __vm_pool + __vm_pool_index++;

  return vm;
}

vaddr_t
vm_page_map_p3 (struct vm_page *vm)
{

  vm->va = vm_base;
  vm_base += PAGE_SIZE;

  return vm->va;
}

struct vm_page *
vm_lookup_by_paddr (paddr_t paddr)
{
  int i;

  for (i = 0; i < 1024; i++)
    if (__vm_pool[i].pa == paddr)
      {
	iprintf ("%s pa %x -> va %x\n", __FUNCTION__,
		 __vm_pool[i].pa, __vm_pool[i].va);
	return __vm_pool + i;
      }

  return 0;
}

void
vm_page_register (struct vm_page *vm, vaddr_t va, paddr_t pa)
{

  vm->va = va;
  vm->pa = pa;
}

vaddr_t
vm_vaddr (struct vm_page *vm)
{

  return vm->va;
}
