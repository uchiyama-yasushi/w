
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
#include <sys/types.h>
#include <sys/console.h>

#include <pcat_mbr.h>
#include <sys/disklabel_netbsd.h>

#include <stdlib.h>
#include <string.h>

#include <sysvbfs/bfs.h>
#include <pcat/bios.h>
#include "bootloader.h"
#include "mbr.h"
#include "disklabel.h"

void
netbsd_disklabel_init ()
{
  struct disklabel *disklabel = (struct disklabel *)bootloader.work.scratch;
  int i;

  // Find NetBSD MBR-partition.
  for (i = 0; i < bootloader.n_mbr_partition; i++)
    {
      struct bootloader_mbr *conf = bootloader.mbr + i;
      struct mbr_partition *mbr = &conf->partition;
      if (mbr->partition_id != 169)
	continue;
      bootloader_drive_change (conf->drive);
      bootloader.work.io->read (bootloader.work.io->cookie,
				bootloader.work.scratch,
				mbr->lba_start_sector + 1);
      if (disklabel->d_magic != DISKMAGIC)
	{
	  printf ("no disklabel NetBSD partition.\n");
	  continue;
	}
      if (bootloader.ndisklabel > NETBSD_DISKLABEL_MAX)
	{
	  printf ("too many disklabel %d > %d\n", bootloader.ndisklabel,
		  NETBSD_DISKLABEL_MAX);
	  break;
	}
      bootloader.netbsd[bootloader.ndisklabel].disklabel = *disklabel;
      bootloader.netbsd[bootloader.ndisklabel].mbr = conf;
      bootloader.ndisklabel++;
    }
}

bool
netbsd_partition_foreach (struct disklabel *d,
			  bool (*func) (struct partition *, int))
{
  struct partition *p = d->d_partitions;
  int i;

  for (i = 0; i < d->d_npartitions; i++, p++)
    {
      if (func (p, i))
	return i;
    }

  return -1;
}

bool
netbsd_partition_print (struct partition *p, int i)
{

  printf ("%d: type %d size %d offset %d\n", i,  p->p_fstype, p->p_size,
	  p->p_offset);

  return FALSE;
}

bool
netbsd_partition_find_sysvbfs (struct partition *p, int i)
{

  return p->p_fstype == 25;	// SysVBFS
}
