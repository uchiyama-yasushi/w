/*	$NetBSD: bfs.c,v 1.11 2008/04/28 20:24:02 martin Exp $	*/

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

#include <string.h>

#include <sysvbfs/bfs.h>

#ifdef BFS_DEBUG
#define	DPRINTF(on, fmt, args...)	if (on) printf(fmt, ##args)
#else
#define	DPRINTF(arg...)		((void)0)
#endif

STATIC uint8_t bfs_tmpbuf[DEV_BSIZE];

int
bfs_init_superblock (struct bfs *bfs, uint32_t bfs_sector,
		     size_t *required_memory)
{
  struct bfs_super_block super;

  bfs->start_sector = bfs_sector;

  /* Read super block */
  if (!bfs->io->read (bfs->io->cookie, (uint8_t *)&super, bfs_sector))
    return EIO;

  if (!bfs_superblock_valid (&super))
    return EINVAL;

  /* i-node table size */
  bfs->data_start = BFSVAL32 (super.header.data_start_byte);
  bfs->data_end = BFSVAL32 (super.header.data_end_byte);

  bfs->max_inode = (bfs->data_start - sizeof (struct bfs_super_block)) /
    sizeof(struct bfs_inode);

  *required_memory = ROUND_SECTOR (bfs->data_start);

  return 0;
}

int
bfs_init_inode (struct bfs *bfs, uint8_t *p, size_t *required_memory)
{
  struct bfs_inode *inode, *root_inode;
  uint32_t i;

  if (!bfs->io->read_n (bfs->io->cookie, p, bfs->start_sector,
			bfs->data_start >> DEV_BSHIFT))
    return EIO;

  bfs->super_block = (struct bfs_super_block *)p;
  bfs->inode = (struct bfs_inode *)(p + sizeof (struct bfs_super_block));
  p += bfs->data_start;

  bfs->n_inode = 0;
  inode = bfs->inode;
  root_inode = 0;
  for (i = 0; i < bfs->max_inode; i++, inode++)
    {
      if (BFSVAL16 (inode->number) != 0)
	{
	  bfs->n_inode++;
	  if (BFSVAL16 (inode->number) == BFS_ROOT_INODE)
	    root_inode = inode;
	}
    }
  DPRINTF (bfs->debug, "inode: %d/%d\n", bfs->n_inode, bfs->max_inode);

  if (root_inode == 0)
    {
      DPRINTF (bfs->debug, "no root directory.\n");
      return ENOTDIR;
    }
  /* dirent table size */
  DPRINTF (bfs->debug, "root inode: %d-%d\n",
	   BFSVAL32 (root_inode->start_sector),
	   BFSVAL32 (root_inode->end_sector));
  bfs->root_inode = root_inode;

  *required_memory = (BFSVAL32 (root_inode->end_sector) -
		      BFSVAL32 (root_inode->start_sector) + 1) << DEV_BSHIFT;

  return 0;
}

int
bfs_init_dirent (struct bfs *bfs, uint8_t *p)
{
  struct bfs_dirent *file;
  struct bfs_inode *inode = bfs->root_inode;
  uint32_t i, n;

  n = BFSVAL32 (inode->end_sector) - BFSVAL32 (inode->start_sector) + 1;

  if (!bfs->io->read_n (bfs->io->cookie, p,
			bfs->start_sector + BFSVAL32 (inode->start_sector), n))
    return EIO;

  bfs->dirent = (struct bfs_dirent *)p;
  bfs->max_dirent = (n << DEV_BSHIFT) / sizeof(struct bfs_dirent);

  file = bfs->dirent;
  bfs->n_dirent = 0;
  for (i = 0; i < bfs->max_dirent; i++, file++)
    if (BFSVAL16 (file->inode) != 0)
      bfs->n_dirent++;

  DPRINTF (bfs->debug, "dirent: %d/%d\n", bfs->n_dirent, bfs->max_dirent);

  return 0;
}

int
bfs_file_read (const struct bfs *bfs, const char *fname, void *buf, size_t bufsz,
	       size_t *read_size)
{
  uint32_t start, end, n;
  size_t sz;
  uint8_t *p;

  if (!bfs_file_lookup (bfs, fname, &start, &end, &sz))
    return ENOENT;

  if (sz > bufsz)
    return ENOMEM;

  p = buf;
  n = end - start;

  if (!bfs->io->read_n (bfs->io->cookie, p, start, n))
    return EIO;

  /* last sector */
  n *= DEV_BSIZE;
  if (!bfs->io->read (bfs->io->cookie, bfs_tmpbuf, end))
    return EIO;
  memcpy(p + n, bfs_tmpbuf, sz - n);

  if (read_size)
    *read_size = sz;

  return 0;
}

int
bfs_file_write (struct bfs *bfs, const char *fname, void *buf,
		size_t bufsz)
{
  struct bfs_fileattr attr;
  struct bfs_dirent *dirent;
  char name[BFS_FILENAME_MAXLEN];
  int err;

  strncpy (name, fname, BFS_FILENAME_MAXLEN);

  if (bfs_dirent_lookup_by_name (bfs, name, &dirent))
    {
      struct bfs_inode *inode;
      if (!bfs_inode_lookup (bfs, BFSVAL16 (dirent->inode), &inode))
	{
	  DPRINTF (bfs->debug, "%s: dirent found, but inode "
		   "not found. inconsistent filesystem.\n",
		   __func__);
	  return ENOENT;
	}
      attr = inode->attr;	/* copy old attribute */
      bfs_file_delete (bfs, name);
      if ((err = bfs_file_create (bfs, name, buf, bufsz, &attr)) != 0)
	return err;
    }
  else
    {
      memset (&attr, 0xff, sizeof attr);	/* Set VNOVAL all */
#if 0	// notyet
      attr.atime = time_second;
      attr.ctime = time_second;
      attr.mtime = time_second;
#endif
      if ((err = bfs_file_create (bfs, name, buf, bufsz, &attr)) != 0)
	return err;
    }

  return 0;
}

int
bfs_file_delete (struct bfs *bfs, const char *fname)
{
  struct bfs_inode *inode;
  struct bfs_dirent *dirent;

  if (!bfs_dirent_lookup_by_name (bfs, fname, &dirent))
    return ENOENT;

  if (!bfs_inode_lookup (bfs, BFSVAL16 (dirent->inode), &inode))
    return ENOENT;

  memset (dirent, 0, sizeof *dirent);
  memset (inode, 0, sizeof *inode);
  bfs->n_inode--;
  bfs->n_dirent--;

  if (!bfs_writeback_dirent (bfs, dirent, FALSE))
    return EIO;
  if (!bfs_writeback_inode (bfs, inode))
    return EIO;
  DPRINTF (bfs->debug, "%s: \"%s\" deleted.\n", __func__, fname);

  return 0;
}

int
bfs_file_rename (struct bfs *bfs, const char *from_name, const char *to_name)
{
  struct bfs_dirent *dirent;
  int err = 0;

  if (strlen (to_name) > BFS_FILENAME_MAXLEN)
    {
      err =  ENAMETOOLONG;
      goto out;
    }
  if (!bfs_dirent_lookup_by_name (bfs, from_name, &dirent))
    {
      err = ENOENT;
      goto out;
    }

  if ((err = bfs_file_delete (bfs, to_name)) != 0)
    goto out;
  strncpy (dirent->name, to_name, BFS_FILENAME_MAXLEN);
  if (!bfs_writeback_dirent (bfs, dirent, FALSE))
    err = EIO;

 out:
  DPRINTF (bfs->debug, "%s: \"%s\" -> \"%s\" error=%d.\n", __func__,
	   from_name, to_name, err);

  return err;
}

int
bfs_file_create (struct bfs *bfs, const char *fname, void *buf, size_t bufsz,
		 const struct bfs_fileattr *attr)
{
  struct bfs_inode *inode;
  struct bfs_dirent *file;
  uint32_t i, j, n, start;
  uint8_t *p;
  int err;

  /* Find free i-node and data block */
  if ((err = bfs_inode_alloc (bfs, &inode, &j, &start)) != 0)
    return err;

  /* File size (unit block) */
  n = (ROUND_SECTOR(bufsz) >> DEV_BSHIFT) - 1;
  if ((int32_t)n < 0)	/* bufsz == 0 */
    n = 0;

  if ((start + n) * DEV_BSIZE >= bfs->data_end)
    {
      DPRINTF (bfs->debug, "disk full.\n");
      return ENOSPC;
    }

  /* Find free dirent */
  for (file = bfs->dirent, i = 0; i < bfs->max_dirent; i++, file++)
    if (BFSVAL16 (file->inode) == 0)
      break;
  if (i == bfs->max_dirent)
    {
      DPRINTF (bfs->debug, "dirent full.\n");
      return ENOSPC;
    }

  /* i-node */
  memset (inode, 0, sizeof *inode);
  inode->number = BFSVAL16 (j);
  inode->start_sector = BFSVAL32 (start);
  inode->end_sector = BFSVAL32 (start + n);
  inode->eof_offset_byte = BFSVAL32 (start * DEV_BSIZE + bufsz - 1);
  /* i-node attribute */
  inode->attr.type = BFSVAL32 ((uint32_t)BFS_FILE_TYPE_FILE);
  inode->attr.mode = BFSVAL32 ((uint32_t)0644);
  inode->attr.nlink = BFSVAL32 ((uint32_t)1);
  bfs_inode_set_attr (bfs, inode, attr);

  /* Dirent */
  memset (file, 0, sizeof *file);
  file->inode = inode->number;
  strncpy (file->name, fname, BFS_FILENAME_MAXLEN);

  DPRINTF (bfs->debug, "%s: start %d end %d\n", __func__,
	   BFSVAL32 (inode->start_sector), BFSVAL32 (inode->end_sector));

  if (buf != 0)
    {
      p = (uint8_t *)buf;
      /* Data block */
      n = 0;
      for (i = BFSVAL32 (inode->start_sector); i < BFSVAL32 (inode->end_sector);
	   i++)
	{
	  if (!bfs->io->write (bfs->io->cookie, p, bfs->start_sector + i))
	    return EIO;
	  p += DEV_BSIZE;
	  n += DEV_BSIZE;
	}
      /* last sector */
      memset (bfs_tmpbuf, 0, DEV_BSIZE);
      memcpy (bfs_tmpbuf, p, bufsz - n);
      if (!bfs->io->write (bfs->io->cookie, bfs_tmpbuf, bfs->start_sector + i))
	return EIO;
    }
  /* Update */
  bfs->n_inode++;
  bfs->n_dirent++;
  if (!bfs_writeback_dirent (bfs, file, TRUE))
    return EIO;
  if (!bfs_writeback_inode (bfs, inode))
    return EIO;

  return 0;
}

bool
bfs_writeback_dirent (const struct bfs *bfs, struct bfs_dirent *dir,
		      bool create)
{
  struct bfs_dirent *dir_base = bfs->dirent;
  struct bfs_inode *root_inode = bfs->root_inode;
  addr_t eof;
  int i;

  i = ((dir - dir_base) * sizeof *dir) >> DEV_BSHIFT;

  eof = (addr_t)(dir + 1) - 1;
  eof = eof - (addr_t)dir_base +
    (BFSVAL32 (root_inode->start_sector) << DEV_BSHIFT);

  /* update root directory inode */
#if 0
  printf ("eof new=%d old=%d\n", eof, root_inode->eof_offset_byte);
#endif
  if (create)
    {
      if (eof > BFSVAL32 (root_inode->eof_offset_byte))
	{
	  root_inode->eof_offset_byte = BFSVAL32 (eof);
	}
    }
  else
    {
      /* delete the last entry */
      if (eof == BFSVAL32 (root_inode->eof_offset_byte))
	{
	  root_inode->eof_offset_byte = BFSVAL32 (eof - sizeof *dir);
	}
    }
  if (!bfs_writeback_inode (bfs, root_inode))
    return FALSE;

  /* update dirent */
  return bfs->io->write (bfs->io->cookie,
			 (uint8_t *)dir_base + (i << DEV_BSHIFT),
			 bfs->start_sector +
			 BFSVAL32 (bfs->root_inode->start_sector) + i);
}

bool
bfs_writeback_inode (const struct bfs *bfs, struct bfs_inode *inode)
{
  struct bfs_inode *inode_base = bfs->inode;
  int i;

  i = ((inode - inode_base) * sizeof *inode) >> DEV_BSHIFT;

  return bfs->io->write (bfs->io->cookie,
			 (uint8_t *)inode_base + (i << DEV_BSHIFT),
			 bfs->start_sector + 1/*super block*/ + i);
}

bool
bfs_file_lookup (const struct bfs *bfs, const char *fname, uint32_t *start,
		 uint32_t *end, size_t *size)
{
  struct bfs_inode *inode;
  struct bfs_dirent *dirent;

  if (!bfs_dirent_lookup_by_name (bfs, fname, &dirent))
    return FALSE;
  if (!bfs_inode_lookup (bfs, BFSVAL16 (dirent->inode), &inode))
    return FALSE;

  if (start)
    *start = BFSVAL32 (inode->start_sector) + bfs->start_sector;
  if (end)
    *end = BFSVAL32 (inode->end_sector) + bfs->start_sector;
  if (size)
    *size = bfs_file_size (inode);

  DPRINTF (bfs->debug, "%s: sector:%d + %d -> %d %dbyte. inode=%d\n",
	   fname, bfs->start_sector, BFSVAL32 (inode->start_sector),
	   BFSVAL32 (inode->end_sector), *size, BFSVAL16 (inode->number));

  return TRUE;
}

bool
bfs_dirent_lookup_by_inode (const struct bfs *bfs, uint32_t inode,
			    struct bfs_dirent **dirent)
{
  struct bfs_dirent *file;
  uint32_t i;

  for (file = bfs->dirent, i = 0; i < bfs->max_dirent; i++, file++)
    if (BFSVAL16 (file->inode) == inode)
      break;

  if (i == bfs->max_dirent)
    return FALSE;

  *dirent = file;

  return TRUE;
}

bool
bfs_dirent_lookup_by_name (const struct bfs *bfs, const char *fname,
			   struct bfs_dirent **dirent)
{
  struct bfs_dirent *file;
  uint32_t i;

  for (file = bfs->dirent, i = 0; i < bfs->max_dirent; i++, file++)
    {
      if ((BFSVAL16 (file->inode) != 0) &&
	  (strncmp (file->name, fname, BFS_FILENAME_MAXLEN) ==0))
	break;
    }

  if (i == bfs->max_dirent)
    return FALSE;

  *dirent = file;

  return TRUE;
}

bool
bfs_inode_lookup (const struct bfs *bfs, ino_t n, struct bfs_inode **iinode)
{
  struct bfs_inode *inode;
  uint32_t i;

  for (inode = bfs->inode, i = 0; i < bfs->max_inode; i++, inode++)
    if (BFSVAL16 (inode->number) == n)
      break;

  if (i == bfs->max_inode)
    return FALSE;

  *iinode = inode;

  return TRUE;
}

size_t
bfs_file_size (const struct bfs_inode *inode)
{

  return BFSVAL32 (inode->eof_offset_byte) -
    BFSVAL32 (inode->start_sector) * DEV_BSIZE + 1;
}

int
bfs_inode_alloc (const struct bfs *bfs, struct bfs_inode **free_inode,
		 uint32_t *free_inode_number, uint32_t *free_block)
{
  struct bfs_inode *jnode, *inode;
  uint32_t i, j, start;

  j = start = 0;
  inode = bfs->inode;
  jnode = 0;

  for (i = BFS_ROOT_INODE; i < bfs->max_inode; i++, inode++) {
    /* Steal i-node # */
    if (j == 0)
      j = i;

    /* Get free i-node */
    if (jnode == 0 && (BFSVAL16 (inode->number) == 0))
      jnode = inode;

    /* Get free i-node # and data block */
    if (BFSVAL16 (inode->number) != 0)
      {
	if (BFSVAL32 (inode->end_sector) > start)
	  start = BFSVAL32 (inode->end_sector);
	if (BFSVAL16 (inode->number) == j)
	  j = 0;	/* conflict */
      }
  }
  start++;

  if (jnode ==  0)
    {
      DPRINTF (bfs->debug, "i-node full.\n");
      return ENOSPC;
    }

  if (start * DEV_BSIZE >= bfs->data_end)
    {
      DPRINTF (bfs->debug, "data block full.\n");
      /* compaction here ? */
      return ENOSPC;
    }
  if (free_inode)
    *free_inode = jnode;
  if (free_inode_number)
    *free_inode_number = j;
  if (free_block)
    *free_block = start;

  return 0;
}

void
bfs_inode_set_attr (const struct bfs *bfs, struct bfs_inode *inode,
		    const struct bfs_fileattr *from)
{
  struct bfs_fileattr *to = &inode->attr;

  if (from != NULL)
    {
      if (from->uid != (sysvbfs_uid_t)-1)
	to->uid = from->uid;
      if (from->gid != (sysvbfs_uid_t)-1)
	to->gid = from->gid;
      if (from->mode != (sysvbfs_mode_t)-1)
	to->mode = from->mode;
      if (from->atime != -1)
	to->atime = from->atime;
      if (from->ctime != -1)
	to->ctime = from->ctime;
      if (from->mtime != -1)
	to->mtime = from->mtime;
    }
  bfs_writeback_inode (bfs, inode);
}

bool
bfs_superblock_valid (const struct bfs_super_block *super)
{

  return super->header.magic == BFSVAL32 (BFS_MAGIC);
}

#ifdef BFS_DEBUG
bool
bfs_dump (const struct bfs *bfs)
{
  const struct bfs_super_block_header *h;
  const struct bfs_compaction *compaction;
  const struct bfs_inode *inode;
  struct bfs_dirent *file;
  uint32_t i, j, s, e;
  size_t bytes;

  if (!bfs_superblock_valid (bfs->super_block))
    {
      DPRINTF(bfs->debug, "invalid bfs super block.\n");
      return FALSE;
    }
  h = &bfs->super_block->header;
  compaction = &bfs->super_block->compaction;

  DPRINTF (bfs->debug, "super block %dbyte, inode %dbyte, dirent %dbyte\n",
	  sizeof *bfs->super_block, sizeof *inode, sizeof *file);

  DPRINTF (bfs->debug, "magic=%x\n", BFSVAL32 (h->magic));
  DPRINTF (bfs->debug, "data_start_byte=0x%x\n", BFSVAL32 (h->data_start_byte));
  DPRINTF (bfs->debug, "data_end_byte=0x%x\n", BFSVAL32 (h->data_end_byte));
  DPRINTF (bfs->debug, "from=%x\n", BFSVAL32 (compaction->from));
  DPRINTF (bfs->debug, "to=%x\n", BFSVAL32 (compaction->to));
  DPRINTF (bfs->debug, "from_backup=%x\n", BFSVAL32 (compaction->from_backup));
  DPRINTF (bfs->debug, "to_backup=%x\n", BFSVAL32 (compaction->to_backup));
  DPRINTF (bfs->debug, "fsname=%s\n", bfs->super_block->fsname);
  DPRINTF (bfs->debug, "volume=%s\n", bfs->super_block->volume);

  /* inode list */
  DPRINTF (bfs->debug, "[inode index list](max %d)\n",
	   bfs->max_inode);
  for (inode = bfs->inode, i = j = 0; i < bfs->max_inode; inode++, i++)
    {
      if (BFSVAL16 (inode->number) != 0)
	{
	  const struct bfs_fileattr *attr = &inode->attr;
	  DPRINTF (bfs->debug, "%d  %d %d %d (%d) ",
		   BFSVAL16 (inode->number),
		   BFSVAL32 (inode->eof_offset_byte) -
		   (BFSVAL32 (inode->start_sector) * DEV_BSIZE) + 1,/*file size*/
		   BFSVAL32 (inode->start_sector),
		   BFSVAL32 (inode->end_sector), i);

	  DPRINTF (bfs->debug, "%d %d %d %d %d %x %x %x\n",
		   BFSVAL32 (attr->type), BFSVAL32 (attr->mode),
		   BFSVAL32 (attr->uid), BFSVAL32 (attr->gid),
		   BFSVAL32 (attr->nlink), BFSVAL32 (attr->atime),
		   BFSVAL32 (attr->mtime), BFSVAL32 (attr->ctime));
	  j++;
	}
    }
  if (j != bfs->n_inode)
    {
      DPRINTF (bfs->debug, "inconsistent cached data. (i-node)\n");
      return FALSE;
    }
  DPRINTF (bfs->debug, "total %d i-node.\n", j);

  /* file list */
  DPRINTF (bfs->debug, "[dirent index list]\n");
  DPRINTF (bfs->debug, "%d file entries.\n", bfs->max_dirent);
  file = bfs->dirent;
  for (i = j = 0; i < bfs->max_dirent; i++, file++)
    {
      if (BFSVAL16 (file->inode) != 0)
	{
	  bfs_file_lookup (bfs, file->name, &s, &e, &bytes);
	  j++;
	}
    }
  if (j != bfs->n_dirent)
    {
      DPRINTF (bfs->debug, "inconsistent cached data. (dirent)\n");
      return FALSE;
    }
  DPRINTF (bfs->debug, "%d files.\n", j);

  return TRUE;
}
#endif
