
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
#include <string.h>
#include <machine/elf.h>

#ifdef DEBUG
#define	DPRINTF(fmt, args...)	printf(fmt, ##args)
#else
#define	DPRINTF(arg...)		((void)0)
#endif

bool
load_elf (uint8_t *buf, uint32_t *entry)
{
  Elf32_Ehdr *e = (void *)buf;
  Elf32_Phdr *p;

  if (e->e_ident[EI_MAG2] != 'L' || e->e_ident[EI_MAG3] != 'F' ||
      e->e_ident[EI_CLASS] != ELFCLASS32 ||
      e->e_type != ET_EXEC
      || e->e_ident[EI_DATA] != ELFDATA2LSB
      || e->e_machine != EM_386)
    {
      printf ("Bad ELF signature.\n");
      return FALSE;
    }

  assert (e->e_phentsize == sizeof(Elf32_Phdr));
  p = (void *)(buf + e->e_phoff);

  DPRINTF ("ELF entry point 0x%x\n", e->e_entry);
  DPRINTF ("# of program header: %d\n", e->e_phnum);
  int i;
  for (i = 0; i < e->e_phnum; i++, p++)
    {
      DPRINTF ("[%d] vaddr=0x%x memsz=0x%x paddr=0x%x offset=0x%x filesz=0x%x\n",
	       i, p->p_vaddr, p->p_memsz, p->p_paddr, p->p_offset, p->p_filesz);
      memcpy ((void *)p->p_paddr, buf + p->p_offset, p->p_filesz);
    }

  *entry = e->e_entry;

  return TRUE;
}
