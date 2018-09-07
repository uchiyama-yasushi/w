/*	$NetBSD: bfs_subr.c,v 1.3 2008/04/28 20:23:18 martin Exp $	*/

/*-
 * Copyright (c) 2004, 2009 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by UCHIYAMA Yasushi.
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
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/system.h>
#include <sys/console.h>

#include <sysvbfs/bfs.h>

#ifdef BFS_DEBUG
#define	DPRINTF(on, fmt, args...)	if (on) printf(fmt, ##args)
#else
#define	DPRINTF(arg...)		((void)0)
#endif

int
bfs_init (struct bfs_init_arg *init)
{
  struct bfs *bfs = init->bfs;
  size_t memsize;
  int error;

  // Setup I/O
  bfs->io = init->io_ops;
  bfs->debug = init->debug;

  /* 1. Read superblock */
  if ((error = bfs_init_superblock (bfs, init->start_sector, &memsize)) != 0)
    {
      printf ("not a bfs superblock (%d)\n", error);
      return EINVAL;
    }
  DPRINTF (bfs->debug, "bfs super block + inode area = %dbyte\n", memsize);
  bfs->super_block_size = memsize;

  if (memsize > init->superblock_size)
    {
      printf ("not enuf superblock buffer. %d > %d\n", memsize,
	      init->superblock_size);
      return ENOMEM;
    }

  /* 2. Setup inode */
  if ((error = bfs_init_inode (bfs, init->superblock, &memsize)) != 0)
    {
      printf ("broken inode.(%d)\n", error);
      return EINVAL;
    }
  DPRINTF (bfs->debug, "bfs dirent area = %dbyte\n", memsize);
  bfs->dirent_size = memsize;

  if (memsize > init->dirent_size)
    {
      printf ("not enuf dirent buffer. %d > %d\n", memsize,
	      init->dirent_size);
      return ENOMEM;
    }

  /* 3. Setup dirent */
  if ((error = bfs_init_dirent(bfs, init->dirent)) != 0)
    {
      printf ("broken dirent.(%d)\n", error);
      return EINVAL;
    }

#ifdef BFS_DEBUG
  bfs_dump(bfs);
#endif

  return 0;
}
