
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

// Virtual vector support for ROM-monitor.

#include <sys/system.h>
#include <sys/console.h>
#include <vector_virtual.h>
#include <vector_name.h>
#include <intc.h>	// VECTOR_MIN, VECTOR_MAX

void
vector_table_init ()
{
  // Install ROM monitor interrupt handler.
  vector_table_update ((const addr_t)vector_table_virtual, TRUE, FALSE);
}

void
vector_table_update (const addr_t virtual_vector_table_addr, bool override,
		     bool verbose)
{
  extern uint32_t vector_link_table_start[];
  int i;
  uint32_t *jmp = vector_link_table_start;
  const addr_t *vec = (const addr_t *)virtual_vector_table_addr;
  uint32_t unused = (uint32_t)(addr_t)null_handler | 0x5a000000;
  // 0x5a000000 is jmp instruction.

  // Install interrupt handler.
  for (i = VECTOR_MIN; i <= VECTOR_MAX; i++, vec++, jmp++)
    {
      if (override || *jmp == unused)
	{
	  *jmp = (*vec & 0xffffff) | 0x5a000000;
	  if (verbose)
	    iprintf ("load 0x%lx (%s)\n", *vec & 0xffffff, vector[i].name);
	}
      else
	{
	  if (verbose)
	    iprintf ("skip %s\n", vector[i].name);// using ROM-monitor.
	}
    }
}
