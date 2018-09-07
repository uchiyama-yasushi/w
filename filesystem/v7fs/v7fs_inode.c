/*	$NetBSD$	*/

/*-
 * Copyright (c) 2011 UCHIYAMA Yasushi.  All rights reserved.
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
__KERNEL_RCSID(0, "$NetBSD$");
#if defined _KERNEL_OPT
#include "opt_v7fs.h"
#endif

#ifdef _KERNEL
#include <sys/systm.h>
#include <sys/param.h>
#else
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#endif

#include "v7fs.h"
#include "v7fs_impl.h"
#include "v7fs_endian.h"
#include "v7fs_inode.h"
#include "v7fs_superblock.h"

#ifdef V7FS_INODE_DEBUG
#define	DPRINTF(fmt, args...)	printf("%s: " fmt, __func__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

void v7fs_inode_setup_disk_image (const struct v7fs_self *, struct v7fs_inode *,
				  struct v7fs_inode_diskimage *);
int v7fs_inode_inquire_disk_location (const struct v7fs_self *, v7fs_ino_t,
				      v7fs_daddr_t *, v7fs_daddr_t *);
int v7fs_inode_block_sanity (const struct v7fs_superblock *, v7fs_daddr_t);

int
v7fs_inode_block_sanity (const struct v7fs_superblock *sb, v7fs_daddr_t blk)
{

  if ((blk < V7FS_ILIST_SECTOR) || (blk >= sb->datablock_start_sector))
    {
      DPRINTF ("invalid inode block#%d (%d-%d)\n", blk, V7FS_ILIST_SECTOR,
	       sb->datablock_start_sector);
      return ENOSPC;
    }

  return 0;
}

int
v7fs_inode_number_sanity (const struct v7fs_superblock *sb, v7fs_ino_t ino)
{

  if (ino < V7FS_ROOT_INODE || ((size_t)ino >= V7FS_MAX_INODE (sb)))
    {
      DPRINTF ("invalid inode#%d (%d-%ld)\n", ino,
	       V7FS_ROOT_INODE, (long)V7FS_MAX_INODE (sb));
      return ENOSPC;
    }

  return 0;
}

int
v7fs_inode_allocate (struct v7fs_self *_, v7fs_ino_t *ino)
{
  struct v7fs_superblock *sb = &_->superblock;
  v7fs_ino_t inode_number;
  int error = 0;
  *ino = 0;

  SUPERB_LOCK (_);
  if (sb->total_freeinode == 0)
    {
      printf ("inode exhausted!\n");
      error = ENOSPC;
      goto errexit;
    }

  // If there is no free inode cache, update it.
  if (sb->nfreeinode <= 0 && (error = v7fs_freeinode_update (_)))
    {
      printf ("inode exhausted!\n");
      goto errexit;
    }
  // Get inode from superblock cache.
  //  assert (sb->nfreeinode <= V7FS_MAX_FREEINODE);
  inode_number = sb->freeinode[--sb->nfreeinode];
  sb->total_freeinode--;
  sb->modified = 1;

  if ((error = v7fs_inode_number_sanity (sb, inode_number)))
    {
      printf ("new inode#%d %d %d\n", inode_number, sb->nfreeinode,
	      sb->total_freeinode);
      printf ("free inode list corupt\n");
      goto errexit;
    }

  *ino = inode_number;

 errexit:
  SUPERB_UNLOCK (_);

  return error;
}

void
v7fs_inode_deallocate (struct v7fs_self *_, v7fs_ino_t ino)
{
  struct v7fs_superblock *sb = &_->superblock;
  struct v7fs_inode inode;

  memset (&inode, 0, sizeof (struct v7fs_inode));
  inode.inode_number = ino;
  v7fs_inode_writeback (_, &inode);

  SUPERB_LOCK (_);
  if (sb->nfreeinode < V7FS_MAX_FREEINODE)
    {
      sb->freeinode[sb->nfreeinode++] = ino; // link to freeinode list.
    }
  sb->total_freeinode++;
  sb->modified = true;
  SUPERB_UNLOCK (_);
}

void
v7fs_inode_setup_memory_image (const struct v7fs_self *_ __attribute__((unused)),
			       struct v7fs_inode *mem,
			       struct v7fs_inode_diskimage *disk)
{
#define	_16(x, m)	(mem->m = V7FS_VAL16 (_, (disk->m)))
#define	_32(x, m)	(mem->m = V7FS_VAL32 (_, (disk->m)))
  uint32_t addr;
  int i;

  memset (mem, 0, sizeof *mem);
  _16 (_, mode);
  _16 (_, nlink);
  _16 (_, uid);
  _16 (_, gid);
  _32 (_, filesize);
  _32 (_, atime);
  _32 (_, mtime);
  _32 (_, ctime);

  for (i = 0; i < V7FS_NADDR; i++)
    {
      int j = i * 3; // 3 byte each.
      // expand to 4byte with endian conversion.
      addr = V7FS_VAL24_READ (_, &disk->addr[j]);
      mem->addr[i] = addr;
    }
  mem->device = 0;
  if (v7fs_inode_iscdev (mem) || v7fs_inode_isbdev (mem))
    {
      mem->device = mem->addr[0];
    }

#undef _16
#undef _32
}

void
v7fs_inode_setup_disk_image (const struct v7fs_self *_ __attribute__((unused)),
			     struct v7fs_inode *mem,
			     struct v7fs_inode_diskimage *disk)
{
#define	_16(x, m)	(disk->m = V7FS_VAL16 (_, (mem->m)))
#define	_32(x, m)	(disk->m = V7FS_VAL32 (_, (mem->m)))

  _16 (_, mode);
  _16 (_, nlink);
  _16 (_, uid);
  _16 (_, gid);
  _32 (_, filesize);
  _32 (_, atime);
  _32 (_, mtime);
  _32 (_, ctime);

  int i;
  for (i = 0; i < V7FS_NADDR; i++)
    {
      int j = i * 3; // 3 byte each.
      V7FS_VAL24_WRITE (_, mem->addr[i], disk->addr + j);
    }
#undef _16
#undef _32
}

// Load inode from disk.
int
v7fs_inode_load (struct v7fs_self *_, struct v7fs_inode *p,
		 v7fs_ino_t n)
{
  v7fs_daddr_t blk, ofs;
  struct v7fs_inode_diskimage *di;
  void *buf;

  if (v7fs_inode_inquire_disk_location (_, n, &blk, &ofs) != 0)
    return ENOENT;

  ILIST_LOCK (_);
  if (!(buf = scratch_read (_, blk)))
    {
      ILIST_UNLOCK (_);
      return EIO;
    }
  ILIST_UNLOCK (_);
  di = (struct v7fs_inode_diskimage *)buf;

  // Decode disk address, convert endian.
  v7fs_inode_setup_memory_image (_, p, di + ofs);
  p->inode_number = n;

  scratch_free (_, buf);

  return 0;
}

// Write back inode to disk.
int
v7fs_inode_writeback (struct v7fs_self *_, struct v7fs_inode *mem)
{
  struct v7fs_inode_diskimage disk;
  v7fs_ino_t ino = mem->inode_number;
  v7fs_daddr_t blk;
  v7fs_daddr_t ofs;
  void *buf;
  int error = 0;

  if (v7fs_inode_inquire_disk_location (_, ino, &blk, &ofs) != 0)
    return ENOENT;

  v7fs_inode_setup_disk_image (_, mem, &disk);

  ILIST_LOCK (_);
  if (!(buf = scratch_read (_, blk)))
    {
      ILIST_UNLOCK (_);
      return EIO;
    }
  struct v7fs_inode_diskimage *di = (struct v7fs_inode_diskimage *)buf;
  di[ofs] = disk; // structure copy;
  if (!_->io.write (_->io.cookie, buf, blk))
    error = EIO;
  ILIST_UNLOCK (_);

  scratch_free (_, buf);

  return error;
}

int
v7fs_inode_inquire_disk_location (const struct v7fs_self *_
				  __attribute__((unused)),
				  v7fs_ino_t n,
				  v7fs_daddr_t *block, v7fs_daddr_t *offset)
{
  v7fs_daddr_t ofs, blk;

  ofs = ((n - 1/*inode start from 1*/) *
		 sizeof (struct v7fs_inode_diskimage));
  blk = ofs >> V7FS_BSHIFT;

  *block = blk + V7FS_ILIST_SECTOR;
  *offset = (ofs - blk * V7FS_BSIZE) / sizeof (struct v7fs_inode_diskimage);
#ifdef V7FS_INODE_DEBUG
  return v7fs_inode_block_sanity (&_->superblock, *block);
#else
  return 0;
#endif
}

