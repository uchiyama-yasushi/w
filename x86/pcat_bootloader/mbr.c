
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
#include <sys/board.h>

#include <sys/console.h>
#include <sys/shell.h>
#include <stdlib.h> //atoi
#include <pcat_mbr.h>

#include <pcat/bios.h>

#include "bootloader.h"
#include "mbr.h"

#include <sysvbfs/bfs.h>	//XXX

STATIC bool mbr_read (void);
STATIC bool mbr_write (void);
STATIC bool mbr_writeback_disk (void);

void
mbr_init ()
{
  struct mbr_partition *partition_table;
  uint8_t *scratch = bootloader.work.scratch;
  uint8_t drive;
  bool
  boot_config_setup (struct mbr_partition *p, int index)
  {
    struct bootloader_mbr *conf = bootloader.mbr + bootloader.n_mbr_partition;
    if (p->partition_id != 0)
      {
	conf->index = index;
	conf->drive = drive;
	conf->partition = *p;	// structure copy
	bootloader.n_mbr_partition++;
      }
    return FALSE;
  }

  // Find Disks all.
  for (drive = 0, bootloader.n_drive = 0; drive < PHISICAL_DISK_MAX; drive++)
    {
      if (!bootloader_drive_change (drive))
	continue;
      if (bootloader.n_drive == DRIVE_MAX)
	{
	  printf ("*** bootloader.mbr over full. ***\n");
	  break;
	}
      bootloader.n_drive++;

      // Read MBR
      bootloader.work.io->read (bootloader.work.io->cookie, scratch, 0);

      if ((partition_table = mbr_partition_table (scratch)) == NULL)
	continue;
      mbr_partition_foreach (partition_table, boot_config_setup);
    }
  bootloader.work.setuped = TRUE;
}

void
mbr_partition_id (int boot_index, int id)
{
  assert (bootloader.work.setuped);

  bootloader.mbr[boot_index].partition.partition_id = id;

  mbr_writeback_disk ();
}

void
mbr_partition_activate (int boot_index)	// bootloader_mbr index.
{
  assert (bootloader.work.setuped);
  int i;
  for (i = 0; i < bootloader.n_mbr_partition; i++)
    {
      struct bootloader_mbr *conf = bootloader.mbr + i;
      conf->partition.boot_indicator = (i == boot_index) ? 0x80 : 0;
      if (conf->partition.boot_indicator)
	{
	  printf ("drive %d, partition %d: activated.\n", conf->drive,
		  conf->index);
	}
    }

  mbr_writeback_disk ();
}

uint32_t
mbr_chain_boot (int boot_index)
{
  assert (bootloader.work.setuped);
  struct bootloader_mbr *conf = bootloader.mbr + boot_index;

  bootloader_drive_change (conf->drive);

  printf ("load sector %d, jump 0x%x\n", conf->partition.lba_start_sector,
	  PCAT_BIOS_LOAD_ADDR);

  if (!bootloader.work.io->read (bootloader.work.io->cookie,
			    (uint8_t *)PCAT_BIOS_LOAD_ADDR,
			    conf->partition.lba_start_sector))
    {
      printf ("failed.\n");
      return 1;
    }

  call16 ((bios_func_t *)PCAT_BIOS_LOAD_ADDR, 0, conf->drive | 0x80);
  // NOTREACHED
  return 2;
}

bool
mbr_writeback_disk ()
{
  assert (bootloader.work.setuped);
  // Writeback to MBR Area.
  struct mbr_partition *partition_table;
  uint8_t drive;
  int i;

  for (drive = 0; drive < bootloader.n_drive; drive++)
    {
      bootloader_drive_change (drive);
      mbr_read ();
      partition_table = (struct mbr_partition *)
	(bootloader.work.scratch + 0x1be);
      for (i = 0; i < bootloader.n_mbr_partition; i++)
	{
	  struct bootloader_mbr *conf = bootloader.mbr + i;
	  if (conf->drive != drive)
	    continue;
	  partition_table[conf->index] = conf->partition;
	}
      mbr_write ();
    }

  return TRUE;
}

bool
mbr_read ()
{

  return bootloader.work.io->read (bootloader.work.io->cookie,
				   bootloader.work.scratch, 0);
}

bool
mbr_write ()
{
  return bootloader.work.io->write (bootloader.work.io->cookie,
				    bootloader.work.scratch, 0);
}

void
mbr_print ()
{
  struct bootloader_mbr *c = bootloader.mbr;
  int i;

  printf ("index|active|drive#|partition#|start|size|partition ID\n");
  for (i = 0; i < bootloader.n_mbr_partition; i++, c++)
    {
      struct mbr_partition *p = &c->partition;
      printf ("%d: %c %d %d [%d %d] ", i,  p->boot_indicator ? '*' : '-',
	      c->drive, c->index, p->lba_start_sector, p->lba_nsector);
      mbr_partition_id_print (p, -1);
    }
}
