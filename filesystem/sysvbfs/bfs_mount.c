
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
#include <filesystem/disklabel/disklabel.h>
#include <filesystem/sysvbfs/bfs.h>
#include <filesystem/sysvbfs/bfs_mount.h>

int
bfs_mount (struct bfs *bfs,
	   const struct block_io_ops *io, uint8_t drive,
	   uint8_t *scratch/* 1024+512byte required*/)
{
  assert (io && scratch);
  uint8_t *bfs_superblock = scratch; // 1024byte (superblock + ilist)
  uint8_t *bfs_dirent = scratch + BFS_SUPERBLOCK_SIZE; // 512byte
  int error;

  io->drive (io->cookie, drive);

  if ((error = disklabel_partition_info (io, scratch, 25/*SysVBFS*/)))
    {
      return ENOENT;
    }

  const struct partition *p = (const struct partition *)scratch;
  uint32_t bfs_start_sector = MBRVAL32 (p->p_offset);
  printf ("fstype %d sector = %d size = %d\n", 25,
	  bfs_start_sector, MBRVAL32 (p->p_size));

  // Initialize SysVBFS ops.
  struct bfs_init_arg init_arg;

  init_arg.start_sector = bfs_start_sector;
  init_arg.superblock_size = BFS_SUPERBLOCK_SIZE;
  init_arg.superblock = bfs_superblock;
  init_arg.dirent_size = BFS_DIRENT_SIZE;
  init_arg.dirent = bfs_dirent;
  init_arg.io_ops = io;
  init_arg.debug = TRUE;
  init_arg.bfs = bfs;

  if ((error = bfs_init (&init_arg)))
    {
      printf ("bfs failed. error=%d\n", error);
      return error;
    }

  return 0;
}
