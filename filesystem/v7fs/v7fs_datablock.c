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

#include <sys/types.h>
#ifdef _KERNEL
#include <sys/systm.h>
#include <sys/param.h>
#else
#include <stdio.h>
#include <string.h>
#include <errno.h>
#endif

#include "v7fs.h"
#include "v7fs_impl.h"
#include "v7fs_endian.h"
#include "v7fs_inode.h"
#include "v7fs_datablock.h"
#include "v7fs_superblock.h"

#ifdef V7FS_DATABLOCK_DEBUG
#define	DPRINTF(fmt, args...)	printf("%s: " fmt, __func__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

struct v7fs_daddr_map
{
  int level; // direct, index1, index2, index3
  v7fs_daddr_t index[3];
};

int v7fs_datablock_addr (size_t, struct v7fs_daddr_map *);
static int v7fs_datablock_deallocate (struct v7fs_self *, v7fs_daddr_t);

static int loop1 (struct v7fs_self *, v7fs_daddr_t, size_t *,
		  int (*) (struct v7fs_self *, void *, v7fs_daddr_t, size_t),
		  void *);
static int loop2 (struct v7fs_self *, v7fs_daddr_t, size_t *,
		  int (*) (struct v7fs_self *, void *, v7fs_daddr_t, size_t),
		  void *);
static v7fs_daddr_t link (struct v7fs_self *, v7fs_daddr_t, int);
static v7fs_daddr_t add_leaf (struct v7fs_self *, v7fs_daddr_t, int);

static v7fs_daddr_t unlink (struct v7fs_self *, v7fs_daddr_t, int);
static v7fs_daddr_t remove_leaf (struct v7fs_self *, v7fs_daddr_t, int);
static v7fs_daddr_t remove_self (struct v7fs_self *, v7fs_daddr_t);

#ifdef V7FS_DATABLOCK_DEBUG
void daddr_map_dump (const struct v7fs_daddr_map *);
#else
#define	daddr_map_dump(x)	((void)0)
#endif

bool
datablock_number_sanity (const struct v7fs_self *_, v7fs_daddr_t blk)
{
  const struct v7fs_superblock *sb = &_->superblock;
  bool ok = (blk >= sb->datablock_start_sector) && (blk < sb->volume_size);

#ifdef V7FS_DATABLOCK_DEBUG
  if (!ok)
    {
      DPRINTF ("Bad data block #%d\n", blk);
    }
#endif

  return ok;
}

int
v7fs_datablock_allocate (struct v7fs_self *_, v7fs_daddr_t *block_number)
{
  struct v7fs_superblock *sb = &_->superblock;
  v7fs_daddr_t blk;
  int error = 0;

  *block_number = 0;
  SUPERB_LOCK (_);
  do
    {
      if (!sb->total_freeblock)
	{
	  DPRINTF ("!!!free block exhausted!!!\n");
	  SUPERB_UNLOCK (_);
	  return ENOSPC;
	}

      // Get free block from superblock cache.
      blk = sb->freeblock[--sb->nfreeblock];
      sb->total_freeblock--;
      sb->modified = 1;

      // If nfreeblock is zero, it block is next freeblock link.
      if (sb->nfreeblock == 0)
	{
	  if ((error = v7fs_freeblock_update (_, blk)))
	    {
	      DPRINTF ("no freeblock!\n");
	      return error;
	    }
	  // This freeblock link is no longer required. use as data block.
	}
    }
  while (!datablock_number_sanity (_, blk)); // skip bogus block.(required?)
  SUPERB_UNLOCK (_);

  DPRINTF ("Get freeblock %d\n", blk);
  // Zero clear datablock.
  void *buf;
  if (!(buf = scratch_read (_, blk)))
    return EIO;
  memset (buf, 0, V7FS_BSIZE);
  if (!_->io.write (_->io.cookie, buf, blk))
    error = EIO;
  scratch_free (_, buf);

  if (!error)
    *block_number = blk;

  return error;
}

int
v7fs_datablock_deallocate (struct v7fs_self *_, v7fs_daddr_t blk)
{
  struct v7fs_superblock *sb = &_->superblock;
  void *buf;
  int error = 0;

  if (!datablock_number_sanity (_, blk))
    return EIO;

  // Add to in-core freelist.
  SUPERB_LOCK (_);
  if (sb->nfreeblock < V7FS_MAX_FREEBLOCK)
    {
      sb->freeblock[sb->nfreeblock++] = blk;
      sb->total_freeblock++;
      sb->modified = 1;
      DPRINTF ("n_freeblock=%d\n", sb->total_freeblock);
      SUPERB_UNLOCK (_);
      return 0;
    }

  // No space to push.
  // Make this block to freeblock list. and current cache moved to this.
  if (!(buf = scratch_read (_, blk)))
    {
      SUPERB_UNLOCK (_);
      return EIO;	// Fatal
    }

  struct v7fs_freeblock *fb = (struct v7fs_freeblock *)buf;
  fb->nfreeblock = V7FS_MAX_FREEBLOCK;
  int i;
  for (i = 0; i < V7FS_MAX_FREEBLOCK; i++)
    fb->freeblock[i] = V7FS_VAL32 (_, sb->freeblock[i]);

  if (!_->io.write (_->io.cookie, (uint8_t *)fb, blk))
    {
      error =  EIO;	// Fatal
    }
  else
    {
      // Link. on next allocate, this block is used as datablock,
      // and swap outed freeblock list is restored.
      sb->freeblock[0] = blk;
      sb->nfreeblock = 1;
      sb->total_freeblock++;
      sb->modified = 1;
      DPRINTF ("n_freeblock=%d\n", sb->total_freeblock);
    }
  SUPERB_UNLOCK (_);
  scratch_free (_, buf);

  return error;
}

int
v7fs_datablock_addr (size_t sz, struct v7fs_daddr_map *map)
{
#define	NIDX		(V7FS_BSIZE / sizeof (v7fs_daddr_t))
#define	DIRECT_SZ	(V7FS_NADDR_DIRECT * V7FS_BSIZE)
#define	IDX1_SZ		(NIDX * V7FS_BSIZE)
#define	IDX2_SZ		(NIDX * NIDX * V7FS_BSIZE)
#define	ROUND(x, a)	((((x) + ((a) - 1)) & ~((a) - 1)))
  if (!sz)
    {
      map->level = 0;
      map->index[0] = 0;
      return 0;
    }

  sz = V7FS_ROUND_BSIZE (sz);

  // Direct
  if (sz <= DIRECT_SZ)
    {
      map->level = 0;
      map->index[0] = (sz >> V7FS_BSHIFT) - 1;
      return 0;
    }
  // Index 1
  sz -= DIRECT_SZ;

  if (sz <= IDX1_SZ)
    {
      map->level = 1;
      map->index[0] = (sz >> V7FS_BSHIFT) - 1;
      return 0;
    }
  sz -= IDX1_SZ;

  // Index 2
  if (sz <= IDX2_SZ)
    {
      map->level = 2;
      map->index[0] = ROUND (sz, IDX1_SZ) / IDX1_SZ - 1;
      map->index[1] = ((sz - (map->index[0] * IDX1_SZ)) >> V7FS_BSHIFT) - 1;
      return 0;
    }
  sz -= IDX2_SZ;

  // Index 3
  map->level = 3;
  map->index[0] = ROUND (sz, IDX2_SZ) / IDX2_SZ - 1;
  sz -= map->index[0] * IDX2_SZ;
  map->index[1] = ROUND (sz, IDX1_SZ) / IDX1_SZ - 1;
  sz -= map->index[1] * IDX1_SZ;
  map->index[2] = (sz >> V7FS_BSHIFT) - 1;

  return map->index[2] >= NIDX ? ENOSPC : 0;
}


int
v7fs_datablock_foreach
(struct v7fs_self *_, struct v7fs_inode *p,
 int (*func) (struct v7fs_self *, void *, v7fs_daddr_t, size_t), void *ctx)
{
  size_t i;
  v7fs_daddr_t blk, blk2;
  size_t filesize;
  bool last;
  int ret;

  if (!(filesize = v7fs_inode_filesize (p)))
    return 0;
#ifdef V7FS_DATABLOCK_DEBUG
  size_t sz = filesize;
#endif

  // Direct
  for (i = 0; i < V7FS_NADDR_DIRECT; i++, filesize -= V7FS_BSIZE)
    {
      blk = p->addr[i];
      if (!datablock_number_sanity (_, blk))
	{
	  DPRINTF ("inode#%d direct=%ld filesize=%ld\n", p->inode_number,
		   (long)i, (long)sz);
	  return EIO;
	}

      last = filesize <= V7FS_BSIZE;
      if ((ret = func (_, ctx, blk, last ? filesize : V7FS_BSIZE)) != 0)
	return ret;
      if (last)
	return V7FS_ITERATOR_END;
    }

  // Index 1
  blk = p->addr[V7FS_NADDR_INDEX1];
  if (!datablock_number_sanity (_, blk))
    return EIO;

  if ((ret = loop1 (_, blk, &filesize, func, ctx)))
    return ret;

  // Index 2
  blk = p->addr[V7FS_NADDR_INDEX2];
  if (!datablock_number_sanity (_, blk))
    return EIO;

  if ((ret = loop2 (_, blk, &filesize, func, ctx)))
    return ret;

  // Index 3
  blk = p->addr[V7FS_NADDR_INDEX3];
  if (!datablock_number_sanity (_, blk))
    return EIO;

  for (i = 0; i < V7FS_BSIZE / sizeof (v7fs_daddr_t); i++)
    {
      blk2 = link (_, blk, i);
      if (!datablock_number_sanity (_, blk))
	return EIO;

      if ((ret = loop2 (_, blk2, &filesize, func, ctx)))
	return ret;
    }

  return EFBIG;
}

int
loop2 (struct v7fs_self *_, v7fs_daddr_t listblk, size_t *filesize,
       int (*func) (struct v7fs_self *, void *, v7fs_daddr_t, size_t),
       void *ctx)
{
  v7fs_daddr_t blk;
  int ret;
  size_t j;

  for (j = 0; j < V7FS_BSIZE / sizeof (v7fs_daddr_t); j++)
    {
      blk = link (_, listblk, j);
      if (!datablock_number_sanity (_, blk))
	return EIO;
      if ((ret = loop1 (_, blk, filesize, func, ctx)))
	return ret;
    }

  return 0;
}

int
loop1 (struct v7fs_self *_, v7fs_daddr_t listblk, size_t *filesize,
       int (*func) (struct v7fs_self *, void *, v7fs_daddr_t, size_t),
       void *ctx)
{
  v7fs_daddr_t blk;
  bool last;
  int ret;
  size_t k;

  for (k = 0; k < V7FS_BSIZE / sizeof (v7fs_daddr_t); k++,
	 *filesize -= V7FS_BSIZE)
    {
      blk = link (_, listblk, k);
      if (!datablock_number_sanity (_, blk))
	return EIO;
      last = *filesize <= V7FS_BSIZE;
      if ((ret = func (_, ctx, blk, last ? *filesize : V7FS_BSIZE)) != 0)
	return ret;
      if (last)
	return V7FS_ITERATOR_END;
    }

  return 0;
}

v7fs_daddr_t
v7fs_datablock_last (struct v7fs_self *_, struct v7fs_inode *inode,
    v7fs_off_t ofs)
{
  struct v7fs_daddr_map map;
  v7fs_daddr_t blk = 0;
  v7fs_daddr_t *addr = inode->addr;

  // Inquire last data block location.
  if (v7fs_datablock_addr (ofs, &map) != 0)
    return 0;

  switch (map.level)
    {
    case 0: //Direct
      blk = inode->addr[map.index[0]];
      break;
    case 1: //Index1
      blk = link (_, addr[V7FS_NADDR_INDEX1], map.index[0]);
      break;
    case 2: //Index2
      blk = link (_, link (_, addr[V7FS_NADDR_INDEX2], map.index[0]),
		       map.index[1]);
      break;
    case 3: //Index3
      blk = link (_, link (_, link (_, addr[V7FS_NADDR_INDEX3],
						   map.index[0]),
				     map.index[1]),
		       map.index[2]);
      break;
    }

  return blk;
}

int
v7fs_datablock_expand (struct v7fs_self *_, struct v7fs_inode *inode, size_t sz)
{
  size_t old_filesize = inode->filesize;
  size_t new_filesize = old_filesize + sz;
  struct v7fs_daddr_map oldmap, newmap;
  v7fs_daddr_t blk, idxblk;
  int error;
  v7fs_daddr_t old_nblk = V7FS_ROUND_BSIZE (old_filesize) >> V7FS_BSHIFT;
  v7fs_daddr_t new_nblk = V7FS_ROUND_BSIZE (new_filesize) >> V7FS_BSHIFT;

  if (old_nblk == new_nblk)
    {
      inode->filesize += sz;
      v7fs_inode_writeback (_, inode);
      return 0; // no need to expand.
    }
  struct v7fs_inode backup = *inode;
  v7fs_daddr_t required_blk = new_nblk - old_nblk;

  DPRINTF ("%ld->%ld, required block=%d\n", (long)old_filesize,
	   (long)new_filesize, required_blk);

  v7fs_datablock_addr (old_filesize, &oldmap);
  v7fs_daddr_t i;
  for (i = 0; i < required_blk; i++)
    {
      v7fs_datablock_addr (old_filesize + (i+1) * V7FS_BSIZE, &newmap);
      daddr_map_dump (&oldmap);
      daddr_map_dump (&newmap);

      if (oldmap.level != newmap.level)
	{
	  // Allocate index area
	  if ((error = v7fs_datablock_allocate (_, &idxblk)))
	    return error;

	  switch (newmap.level)
	    {
	    case 1:
		    DPRINTF ("0->1\n");
	      inode->addr[V7FS_NADDR_INDEX1] = idxblk;
	      blk = add_leaf (_, idxblk, 0);
	      break;
	    case 2:
		    DPRINTF ("1->2\n");
	      inode->addr[V7FS_NADDR_INDEX2] = idxblk;
	      blk = add_leaf (_, add_leaf (_, idxblk, 0), 0);
	      break;
	    case 3:
		    DPRINTF ("2->3\n");
	      inode->addr[V7FS_NADDR_INDEX3] = idxblk;
	      blk = add_leaf (_, add_leaf (_, add_leaf (_, idxblk, 0), 0), 0);
	      break;
	    }
	}
      else
	{
	  switch (newmap.level)
	    {
	    case 0:
	      if ((error = v7fs_datablock_allocate (_, &blk)))
		return error;
	      inode->addr[newmap.index[0]] = blk;
	      DPRINTF ("direct index %d = blk%d\n", newmap.index[0], blk);
	      break;
	    case 1:
	      idxblk = inode->addr[V7FS_NADDR_INDEX1];
	      blk = add_leaf (_, idxblk, newmap.index[0]);
	      break;
	    case 2:
	      idxblk = inode->addr[V7FS_NADDR_INDEX2];
	      if (oldmap.index[0] != newmap.index[0])
		add_leaf (_, idxblk, newmap.index[0]);
	      blk = add_leaf (_, link (_, idxblk, newmap.index[0]),
			      newmap.index[1]);
	      break;
	    case 3:
	      idxblk = inode->addr[V7FS_NADDR_INDEX3];

	      if (oldmap.index[0] != newmap.index[0])
		add_leaf (_, idxblk, newmap.index[0]);

	      if (oldmap.index[1] != newmap.index[1])
		add_leaf (_, link (_, idxblk, newmap.index[0]),
			  newmap.index[1]);
	      blk = add_leaf (_, link (_, link (_, idxblk,
						newmap.index[0]),
				 newmap.index[1]),
			      newmap.index[2]);
	      break;
	    }
	}
      if (!blk)
	{
	  *inode = backup; // structure copy;
	  return ENOSPC;
	}
      oldmap = newmap;
    }
  inode->filesize += sz;
  v7fs_inode_writeback (_, inode);

  return 0;
}

v7fs_daddr_t
link (struct v7fs_self *_, v7fs_daddr_t listblk, int n)
{
  v7fs_daddr_t *list;
  v7fs_daddr_t blk;
  void *buf;

  if (!datablock_number_sanity (_, listblk))
    return 0;
  if (!(buf = scratch_read (_, listblk)))
    return 0;
  list = (v7fs_daddr_t *)buf;
  blk = V7FS_VAL32 (_, list[n]);
  scratch_free (_, buf);

  if (!datablock_number_sanity (_, blk))
    return 0;

  return blk;
}

v7fs_daddr_t
add_leaf (struct v7fs_self *_, v7fs_daddr_t up, int idx)
{
  v7fs_daddr_t newblk;
  v7fs_daddr_t *daddr_list;
  int error = 0;
  void *buf;

  if (!up)
    return 0;
  if (!datablock_number_sanity (_, up))
    return 0;

  if ((error = v7fs_datablock_allocate (_, &newblk)))
    return 0;
  if (!(buf = scratch_read (_, up)))
    return 0;
  daddr_list = (v7fs_daddr_t *)buf;
  daddr_list[idx] = V7FS_VAL32 (_, (newblk));
  if (!_->io.write (_->io.cookie, buf, up))
    newblk = 0;
  scratch_free (_, buf);

  return newblk;
}

int
v7fs_datablock_contract (struct v7fs_self *_, struct v7fs_inode *inode, size_t sz)
{
  size_t old_filesize = inode->filesize;
  size_t new_filesize = old_filesize - sz;
  struct v7fs_daddr_map oldmap, newmap;
  v7fs_daddr_t blk, idxblk;
  int error = 0;
  v7fs_daddr_t old_nblk = V7FS_ROUND_BSIZE (old_filesize) >> V7FS_BSHIFT;
  v7fs_daddr_t new_nblk = V7FS_ROUND_BSIZE (new_filesize) >> V7FS_BSHIFT;

  if (old_nblk == new_nblk)
    {
      inode->filesize -= sz;
      v7fs_inode_writeback (_, inode);
      return 0; // no need to contract;
    }
  v7fs_daddr_t erase_blk = old_nblk - new_nblk;

  DPRINTF ("%ld->%ld # of erased block=%d\n",
	   (long)old_filesize, (long)new_filesize, erase_blk);

  v7fs_datablock_addr (old_filesize, &oldmap);
  v7fs_daddr_t i;
  for (i = 0; i < erase_blk; i++)
    {
      v7fs_datablock_addr (old_filesize - (i+1) * V7FS_BSIZE, &newmap);
      //      daddr_map_dump (&oldmap);
      //      daddr_map_dump (&newmap);

      if (oldmap.level != newmap.level)
	{
	  switch (newmap.level)
	    {
	    case 0: //1->0
	      DPRINTF ("1->0\n");
	      idxblk = inode->addr[V7FS_NADDR_INDEX1];
	      inode->addr[V7FS_NADDR_INDEX1] = 0;
	      error = v7fs_datablock_deallocate
		(_, remove_self(_, idxblk));
	      break;
	    case 1: //2->1
	      DPRINTF ("2->1\n");
	      idxblk = inode->addr[V7FS_NADDR_INDEX2];
	      inode->addr[V7FS_NADDR_INDEX2] = 0;
	      error = v7fs_datablock_deallocate
		(_, remove_self(_, remove_self(_, idxblk)));
	      break;
	    case 2://3->2
	      DPRINTF ("3->2\n");
	      idxblk = inode->addr[V7FS_NADDR_INDEX3];
	      inode->addr[V7FS_NADDR_INDEX3] = 0;
	      error = v7fs_datablock_deallocate
		(_, remove_self (_, remove_self (_, remove_self (_, idxblk))));
	      break;
	    }
	}
      else
	{
	  switch (newmap.level)
	    {
	    case 0:
	      DPRINTF ("[0] %d\n", oldmap.index[0]);
	      blk = inode->addr[oldmap.index[0]];
	      error = v7fs_datablock_deallocate (_, blk);
	      break;
	    case 1:
	      DPRINTF ("[1] %d\n", oldmap.index[0]);
	      idxblk = inode->addr[V7FS_NADDR_INDEX1];
	      remove_leaf (_, idxblk, oldmap.index[0]);

	      break;
	    case 2:
	      DPRINTF ("[2] %d %d\n", oldmap.index[0], oldmap.index[1]);
	      idxblk = inode->addr[V7FS_NADDR_INDEX2];
	      remove_leaf (_,
			   link (_, idxblk, oldmap.index[0]), oldmap.index[1]);
	      if (oldmap.index[0] != newmap.index[0])
		{
		  //		  assert (oldmap.index[1] == 0);
		  remove_leaf (_, idxblk, oldmap.index[0]);
		}
	      break;
	    case 3:
	      DPRINTF ("[2] %d %d %d\n", oldmap.index[0], oldmap.index[1],
		       oldmap.index[2]);
	      idxblk = inode->addr[V7FS_NADDR_INDEX3];
	      remove_leaf (_, link (_, link (_, idxblk,
					     oldmap.index[0]),
				    oldmap.index[1]),
			   oldmap.index[2]);

	      if (oldmap.index[1] != newmap.index[1])
		{
		  //		  assert (oldmap.index[2] == 0);
		  remove_leaf (_, link (_, idxblk, oldmap.index[0]),
			       oldmap.index[1]);
		}
	      if (oldmap.index[0] != newmap.index[0])
		{
		  //		  assert (oldmap.index[1] == 0);
		  remove_leaf (_, idxblk, oldmap.index[0]);
		}
	      break;
	    }
	}
      oldmap = newmap;
    }
  inode->filesize -= sz;
  v7fs_inode_writeback (_, inode);

  return error;
}

v7fs_daddr_t
unlink (struct v7fs_self *_, v7fs_daddr_t idxblk, int n)
{
  v7fs_daddr_t *daddr_list;
  v7fs_daddr_t blk;
  void *buf;

  if (!(buf = scratch_read (_, idxblk)))
    return 0;
  daddr_list = (v7fs_daddr_t *)buf;
  blk = V7FS_VAL32 (_, daddr_list[n]);
  daddr_list[n] = 0;
  _->io.write (_->io.cookie, buf, idxblk);
  scratch_free (_, buf);

  return blk; // unlinked block.
}

v7fs_daddr_t
remove_self(struct v7fs_self *_, v7fs_daddr_t up)
{
  v7fs_daddr_t down;

  if (!datablock_number_sanity (_, up))
    return 0;

  // At 1st, remove from datablock list.
  down = unlink (_, up, 0);

  // link self to freelist.
  v7fs_datablock_deallocate (_, up);

  return down;
}

v7fs_daddr_t
remove_leaf (struct v7fs_self *_, v7fs_daddr_t up, int n)
{
  v7fs_daddr_t down;

  if (!datablock_number_sanity (_, up))
    return 0;

  // At 1st, remove from datablock list.
  down = unlink (_, up, n);

  // link leaf to freelist.
  v7fs_datablock_deallocate (_, down);

  return down;
}

#ifdef V7FS_DATABLOCK_DEBUG
void
daddr_map_dump (const struct v7fs_daddr_map *map)
{

  DPRINTF ("level %d ", map->level);
  int m, n = !map->level ? 1 : map->level;
  for (m = 0; m < n; m++)
    printf ("[%d]", map->index[m]);
  printf ("\n");
}
#endif
