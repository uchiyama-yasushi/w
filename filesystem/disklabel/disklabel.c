
/*-
 * Copyright (c) 2009, 2011 UCHIYAMA Yasushi.  All rights reserved.
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
#include <sys/block_io.h>
#include <filesystem/mbr/mbr.h>
#include <filesystem/disklabel/disklabel.h>

#include <string.h>

bool find_partition_subr (struct mbr_partition *, int);

bool
find_partition_subr (struct mbr_partition *p, int idx __attribute__((unused)))
{

  return p->partition_id == MBR_NETBSD;
}

int
disklabel_load (const struct block_io_ops *io, uint8_t *scratch)
{

  // Read MBR
  if (!io->read (io->cookie, scratch, 0))
    return EIO;

  struct mbr_partition *partition_table = mbr_partition_table (scratch);
  if (!partition_table)
    {
      printf ("*** no MBR partitions. ***\n");
      return ENOENT;
    }

  // Find NetBSD partition.
  int mbr_idx = mbr_partition_foreach (partition_table, find_partition_subr);
  if (mbr_idx < 0)
    {
      printf ("*** no NetBSD partitions. ***\n");
      return ENOENT;
    }

  // Read NetBSD disklabel.
  daddr_t disklabel_sector =
    MBRVAL32 (partition_table[mbr_idx].lba_start_sector) + LABEL_SECTOR;
  io->read (io->cookie, scratch, disklabel_sector);

  const struct disklabel *d = (const struct disklabel *)scratch;
  if (MBRVAL32 (d->d_magic) != DISKMAGIC)
    {
      printf ("no disklabel NetBSD partition.\n");
      return ENOENT;
    }

  return 0;
}

int
disklabel_partition_info (const struct block_io_ops *io, uint8_t *scratch,
			  int fstype)
{
  int error;

  // Load NetBSD disklabel.
  if ((error = disklabel_load (io, scratch)))
    return error;
  const struct disklabel *d = (const struct disklabel *)scratch;

  // Find partition.
  const struct partition *p = d->d_partitions;
  int n = MBRVAL16 (d->d_npartitions);
  int i;

  assert (sizeof (struct partition) == 16); // detect compiler bug.

  for (i = 0; i < n; i++, p++)
    {
      if (p->p_fstype == fstype)
	break;
    }

  if (i == n)
    {
      printf ("*** no fstype=%d partition.\n", fstype);
      return ENOENT;
    }
  memmove (scratch, p, sizeof *p);

  return 0;
}

