
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

#include <sys/cdefs.h>
#include <stdio.h>
#include <filesystem/mbr/mbr.h>

STATIC const struct
{
  int id;
  const char *name;
} mbr_id [] =
  {
    { 0, "unused" },
    { 1, "DOS 12-bit FAT" },
    { 4, "DOS 3.0+ 16-bit FAT <32M" },
    { 6, "DOS 3.31+ 16-bit FAT >=32M" },
    { 7, "HPFS/NTFS" },
    { 11, "WIN95 OSR2 FAT32" },
    { 12, "WIN95 OSR2 FAT32, LBA-mapped" },
    { 14, "WIN95: DOS 16-bit FAT, LBA-mapped" },
    { 130, "Linux swap" },
    { 131, "Linux native partition" },
    { 133, "Linux extended partition" },
    { 160, "Laptop hibernation partition" },
    { 165, "FreeBSD" },
    { 166, "OpenBSD" },
    { 169, "NetBSD" },
  };

const char *
partition_id_name (int id)
{
  int i, n = sizeof mbr_id / sizeof (mbr_id[0]);

  for (i = 0; i < n; i++)
    if (mbr_id[i].id == id)
      break;
  if (i == n)
    return NULL;

  return mbr_id[i].name;
}

bool
mbr_partition_id_print (struct mbr_partition *p, int idx)
{
  const char *name = partition_id_name (p->partition_id);

  if (idx >= 0)
    printf ("[%d] ", idx);

  if (name)
    printf ("%s\n", name);
  else
    printf ("%d\n", p->partition_id);

  return FALSE;
}

bool
mbr_partition_info (struct mbr_partition *p, int idx)
{
  const char *name = partition_id_name (p->partition_id);
  uint32_t lba_start = MBRVAL32 (p->lba_start_sector);
  uint32_t lba_size = MBRVAL32 (p->lba_nsector);

  if (idx >= 0)
    printf ("[%d] ", idx);

  if (name)
    printf ("%s ", name);
  else
    printf ("%d ", p->partition_id);

  printf ("[%ld %ld]\n",  lba_start, lba_size);

  return FALSE;
}

