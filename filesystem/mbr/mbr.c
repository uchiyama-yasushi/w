
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
#include <filesystem/mbr/mbr.h>

struct mbr_partition *
mbr_partition_table (const uint8_t *sector_image)
{
  uint16_t signature;

  // Check signagure.
  signature = *(volatile uint16_t *)(sector_image + 0x1fe);
  if (signature != MBRVAL16 (0xaa55))
    {
      printf ("%s: MBR signature not found. signature=%x\n",
	      __FUNCTION__, (uint32_t)signature);
      return NULL;
    }

  return (struct mbr_partition *)(sector_image + 0x1be);
}

int
mbr_partition_foreach (struct mbr_partition *mbr,
		       bool (*func) (struct mbr_partition *, int))
{
  struct mbr_partition *p = mbr;
  int i;

  for (i = 0; i < PHISICAL_DISK_MAX; i++, p++)
    {
      if (func (p, i))
	return i;
    }

  return -1;
}

