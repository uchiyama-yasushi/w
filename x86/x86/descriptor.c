
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

#include <sys/types.h>
#include <sys/system.h>
#include <x86/segment.h>

union descriptor *
descriptor (int idx)
{
  struct lgdt_arg gdt;

  __asm volatile ("sgdt %0" : "=m"(gdt):);
  if (idx > gdt.limit)
    panic (__FUNCTION__);

  return (union descriptor *)
    (gdt.base +(idx & ~(SELECTOR_RPL_MASK | SELECTOR_TI)));
}

void
descriptor_set_priority (union descriptor *d, int pri)
{

  d->desc.access_byte &= ~DPL_MASK;
  d->desc.access_byte |= ((pri << DPL_SHIFT) & DPL_MASK);
}

int
descriptor_get_priority (union descriptor *d)
{

  return (d->desc.access_byte >> DPL_SHIFT) & 0x3;
}

void
descriptor_set_base (struct application_descriptor *d, uint32_t base)
{

  d->base_0_15	= base & 0xffff;
  d->base_16_23	= (base >> 16) & 0xff;
  d->base_24_31	= (base >> 24) & 0xff;
}

void
descriptor_set_size (struct application_descriptor *d, uint32_t size)
{

  size--;
  d->size_0_15		= size & 0xffff;
  d->size_16_19_GD	&= ~0xf;
  d->size_16_19_GD	|= (size >> 16) & 0xf;
}

uint32_t
descriptor_get_base (struct application_descriptor *d)
{
  uint32_t base;

  base = d->base_0_15 | d->base_16_23 << 16 | d->base_24_31 << 24;

  return base;
}

uint32_t
descriptor_get_size (struct application_descriptor *d)
{
  uint32_t size;

  size = d->size_0_15 | ((d->size_16_19_GD & 0xf) << 16);
  size++;

  return size;
}
