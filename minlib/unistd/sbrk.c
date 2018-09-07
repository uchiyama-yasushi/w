
/*-
 * Copyright (c) 2011 UCHIYAMA Yasushi.  All rights reserved.
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

#include <minlib.h>
#include <sys/types.h>
#include <stdio.h>

STATIC char *sbrk_heap;
STATIC addr_t sbrk_size;

void
sbrk_set_arena (addr_t base, addr_t size)
{
  if (base)
    sbrk_heap = (char *)base;
  if (size)
    sbrk_size = size;
}

int
LIBFUNC (brk) (void *addr)
{
  // alignment check
  sbrk_heap = (char *)addr;

  return 0;
}

void *
LIBFUNC (sbrk) (int incr)
{
  extern char bss_end[];

  if (!sbrk_heap)
    sbrk_heap = bss_end;

  char *p = sbrk_heap;
  //XXX check stack pointer here.
  sbrk_heap += incr;
  printf ("sbrk:base=%p size=%d\n", p, incr);

  return (void *)p;
}

