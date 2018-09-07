
/*-
 * Copyright (c) 2008-2009 UCHIYAMA Yasushi.  All rights reserved.
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
#include <pcat/bfs_bios.h>
#include <string.h>
#include "gdt.h"

#ifdef DISK_DEBUG
#define	DPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

extern uint8_t bios_fdd_context[];	// bounce buffer.
extern uint8_t bios_hdd_context[];	// bounce buffer.

struct block_io_ops bios_fdd_io =
  {
    NULL,
    bios_fdd_drive,
    bios_fdd_read,
    bios_fdd_read_n,
    bios_fdd_write,
    bios_fdd_write_n
  };

struct block_io_ops bios_hdd_io =
  {
    NULL,
    bios_disk_drive,
    bios_disk_read,
    bios_disk_read_n,
    bios_disk_write,
    bios_disk_write_n
  };

#define	BFS_SUPERBLOCK_SIZE	1024
#define	BFS_DIRENT_SIZE		512
#define	SCRATCH_AREA		(16 * 1024 * 1024)
STATIC uint8_t bfs_superblock[BFS_SUPERBLOCK_SIZE];
STATIC uint8_t bfs_dirent[BFS_DIRENT_SIZE];
STATIC struct bfs booted_drive_bfs;

STATIC uint8_t tmpbuf[512];

struct bfs *
floppy_bfs_init (int drive)
{
  void *fdd_context = (void *)((addr_t)bios_fdd_context | GDT_DATA32_BIOS_BASE);

  // Setup Disk I/O ops.
  if ((bios_fdd_io.cookie = bios_fdd_init (fdd_context, drive)) == NULL)
    return NULL;

  if (!bios_fdd_io.read (bios_fdd_io.cookie, tmpbuf, 1))
    {
      DPRINTF ("couldn't read disklabel.\n");
      return NULL;
    }

  uint32_t start_sector;
  if (!sysvbfs_partition_find ((struct disklabel *)tmpbuf, &start_sector))
    {
      DPRINTF ("couldn't find SysVBFS partition\n");
      return NULL;
    }

  struct bfs_init_arg init_arg;
  struct bfs *b;
  init_arg.start_sector = start_sector;
  init_arg.superblock_size = BFS_SUPERBLOCK_SIZE;
  init_arg.superblock = bfs_superblock;
  init_arg.dirent_size = BFS_DIRENT_SIZE;
  init_arg.dirent = bfs_dirent;
  init_arg.io_ops = &bios_fdd_io;
  init_arg.debug = TRUE;
  init_arg.bfs = &booted_drive_bfs;

  if ((b = bfs_init (&init_arg)) == NULL)
    {
      DPRINTF ("broken filesystem.\n");
      return NULL;
    }

  return b;
}

struct bfs *
disk_bfs_init (int drive)
{
  bool netbsd_disklabel_match (struct mbr_partition *p, int idx)
  {
    return p->partition_id == 169;
  }
  void *hdd_context = (void *)((addr_t)bios_hdd_context | GDT_DATA32_BIOS_BASE);
  int i;

  // Setup Disk I/O ops.
  if ((bios_hdd_io.cookie = bios_disk_init (hdd_context, drive)) == NULL)
    return NULL;
  // Read MBR
  if (!bios_hdd_io.read (bios_hdd_io.cookie, tmpbuf, 0))
    {
      DPRINTF ("couldn't read MBR.\n");
      return NULL;
    }
  // Find NetBSD partition
  struct mbr_partition *mbr = mbr_partition_table (tmpbuf);
  if ((i = mbr_partition_foreach (mbr, netbsd_disklabel_match)) < 0)
    {
      DPRINTF ("no NetBSD partition.\n");
      return NULL;
    }
  // Read disklabel.
  if (!bios_hdd_io.read (bios_hdd_io.cookie, tmpbuf,
			 mbr[i].lba_start_sector + 1)) // +1 skip PBR.
    {
      DPRINTF ("couldn't read disklabel.\n");
      return NULL;
    }
  // Find SysVBFS partition
  uint32_t start_sector;
  if (!sysvbfs_partition_find ((struct disklabel *)tmpbuf, &start_sector))
    {
      DPRINTF ("couldn't find SysVBFS partition\n");
      return NULL;
    }
  // Setup BFS.
  struct bfs_init_arg init_arg;
  struct bfs *b;
  init_arg.start_sector = start_sector;
  init_arg.superblock_size = BFS_SUPERBLOCK_SIZE;
  init_arg.superblock = bfs_superblock;
  init_arg.dirent_size = BFS_DIRENT_SIZE;
  init_arg.dirent = bfs_dirent;
  init_arg.io_ops = &bios_hdd_io;
  init_arg.debug = TRUE;
  init_arg.bfs = &booted_drive_bfs;

  if ((b = bfs_init (&init_arg)) == NULL)
    {
      DPRINTF ("broken filesystem.\n");
      return NULL;
    }

  return b;
}

struct block_io_ops *
disk_init (uint8_t drive)
{
  void *hdd_context = (void *)((addr_t)bios_hdd_context | GDT_DATA32_BIOS_BASE);

  // Setup Disk I/O ops.
  if ((bios_hdd_io.cookie = bios_disk_init (hdd_context, drive)) == NULL)
    return NULL;

  return &bios_hdd_io;
}

void
print_disk_info (int drive)
{
  struct block_io_ops *io = 0;
  struct block_io_ops backup;
  int i, j, start, end;

  if (drive < 0)
    {
      start = 0;
      end = PHISICAL_DISK_MAX;
    }
  else
    {
      start = drive;
      end = start + 1;
    }

  memcpy (&backup, &bios_hdd_io, sizeof backup);
  for (i = start; i < end; i++)
    {
      if ((io = disk_init (0x80 + i)) == NULL)
	continue;

      printf ("[Hard Disk %d]\n", i);
      bios_disk_info (io->cookie, 0x80 + i);
      // Read MBR
      if (!io->read (io->cookie, tmpbuf, 0))
	continue;
      struct mbr_partition *partition;
      if ((partition = mbr_partition_table (tmpbuf)) == NULL)
	continue;

      struct mbr_partition *p = partition;
      for (j = 0; j < 4; j++, p++)
	{
	  const char *name;
	  int id = p->partition_id;
	  printf ("[%d] ", j);
	  if (id)
	    {
	      printf ("%d-%d ", p->lba_start_sector,
		      p->lba_start_sector + p->lba_nsector - 1);
	      if ((name = partition_id_name (id)) != NULL)
		printf ("<%s>\n", name);
	      else
		printf ("%d\n", id);
	    }
	  else
	    {
	      printf ("unused \n");
	    }
	}
    }
  memcpy (&bios_hdd_io, &backup, sizeof backup);
}
