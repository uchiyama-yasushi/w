
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

#include <sysvbfs/bfs.h>

#include <stdlib.h>
#include <string.h>

#include <pcat/bios.h>
#include <machine/elf.h>

#include "bootloader.h"
#include "mbr.h"
#include "disklabel.h"
#include "sysvbfs.h"

#define	BFS_SUPERBLOCK_SIZE	1024
#define	BFS_DIRENT_SIZE		512

STATIC uint8_t bfs_superblock[BFS_SUPERBLOCK_SIZE];
STATIC uint8_t bfs_dirent[BFS_DIRENT_SIZE];

STATIC struct bfs *sysvbfs_prepare (struct bootloader_bfs *);

bool
sysvbfs_init ()
{
  struct bootloader_bfs bb;
  struct bfs *b;
  int sysvbfs_partition;
  int i, n;

  for (i = n = 0; i < bootloader.ndisklabel; i++)
    {
      struct bootloader_disklabel *bd = bootloader.netbsd + i;
      struct disklabel *d = &bd->disklabel;
      struct bootloader_mbr *conf = bd->mbr;

      // Find SysVBFS partition.
      bootloader_drive_change (conf->drive);
      if ((sysvbfs_partition = netbsd_partition_foreach
	   (d, netbsd_partition_find_sysvbfs)) != -1)
	{
	  bb.mbr = conf;
	  bb.partition = d->d_partitions + sysvbfs_partition;
	  if (n > BFS_PARTITION_MAX)
	    {
	      printf ("too may BFS partition. %d > %d\n", n, BFS_PARTITION_MAX);
	      break;
	    }
	  if ((b = sysvbfs_prepare (&bb)) != NULL)
	    {
	      bootloader.bfs[n] = bb;
	      n++;
	    }
	  else
	    {
	      printf ("BFS setup failed.\n");
	    }
	}

    }
  bootloader.nbfs = n;

  return n != 0;
}

struct bfs *
sysvbfs_prepare (struct bootloader_bfs *bb)
{
  struct partition *p = bb->partition;
  struct bfs_init_arg init_arg;
  struct bfs *b;

  init_arg.start_sector = p->p_offset;
  init_arg.superblock_size = BFS_SUPERBLOCK_SIZE;
  init_arg.superblock = bfs_superblock;
  init_arg.dirent_size = BFS_DIRENT_SIZE;
  init_arg.dirent = bfs_dirent;
  init_arg.io_ops = bootloader.work.io;
  init_arg.debug = FALSE;
  init_arg.bfs = &bb->bfs;

  bootloader_drive_change (bb->mbr->drive);
  if ((b = bfs_init (&init_arg)) == NULL)
    return NULL;

  return b;
}

void
sysvbfs_ls (struct bootloader_bfs *bb)
{
  struct bfs_dirent *file;
  struct bfs_inode *inode;
  struct bfs *b = sysvbfs_prepare (bb);
  int i;

  for (file = b->dirent, i = 0; i < b->max_dirent; i++, file++)
    {
      if (file->inode != 0)
	{
	  inode = &b->inode[file->inode - BFS_ROOT_INODE];
	  printf ("%s\t%d (%d-%d)\n", file->name,
		  bfs_file_size(inode), inode->start_sector,
		  inode->end_sector);
	}
    }
}

uint32_t
sysvbfs_boot (struct bootloader_bfs *bb, const char *filename)
{
  addr_t entry;
  size_t readed;
  int err;
  struct bfs *b = sysvbfs_prepare (bb);

  if ((err = bfs_file_read (b, filename, bootloader.work.scratch,
			    0xffffffff, &readed)) != 0)
    {
      printf ("%s: read failed. (%d)\n", filename, err);
      return 1;
    }

  if (!load_elf (bootloader.work.scratch, &entry))
    {
      printf ("%s: not a ELF.\n", filename);
      return 1;
    }
  printf ("filesize=%dbyte. entry=0x%x\n", readed, entry);

  //  call16 (bios_vga_mode, 0x13, 0);
  //  __asm volatile ("jmp *%0":: "r"(entry));
  __asm volatile ("movl %1, %%eax; jmp *%0":: "c"(entry), "g"(boot_drive));
  // NOTREACHED
  return 2;
}
