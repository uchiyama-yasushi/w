
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

// bfs_datafile provides simple sector r/w.

#include <sys/system.h>
#include <sys/console.h>

#include <sysvbfs/bfs.h>
#include <sysvbfs/bfs_datafile.h>

#include <string.h>

#ifdef BFS_DEBUG
#define	DPRINTF(on, fmt, args...)	if (on) printf(fmt, ##args)
#else
#define	DPRINTF(arg...)		((void)0)
#endif

int
bfs_datafile_open (struct bfs *bfs, const char *fname,
		   const struct bfs_fileattr *attr, struct bfs_datafile *f)
{
  struct bfs_inode *inode;
  struct bfs_dirent *file;
  struct bfs_dirent *dirent;
  uint32_t i, j, start;
  int err;

  /* Check filename. */
  if (bfs_dirent_lookup_by_name (bfs, fname, &dirent))
    return EINVAL;

  /* Find free i-node and data block */
  if ((err = bfs_inode_alloc (bfs, &inode, &j, &start)) != 0)
    return err;
  f->inode = inode;
  f->start = f->current = bfs->start_sector + start;
  f->size = 0;
  f->size_changed = FALSE;
  f->bfs = bfs;

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
  inode->end_sector = BFSVAL32 (start);
  inode->eof_offset_byte = BFSVAL32 (start * DEV_BSIZE - 1);

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

  /* Update */
  bfs->n_inode++;
  bfs->n_dirent++;
  bfs_writeback_dirent (bfs, file, TRUE);
  bfs_writeback_inode (bfs, inode);

  return 0;
}

int
bfs_datafile_close (struct bfs_datafile *f)
{
  struct bfs *bfs = f->bfs;
  struct bfs_inode *inode;

  if (!f->size_changed)
    return 0;

  inode = f->inode;
  /* i-node */
  inode->end_sector = BFSVAL32 (f->current - bfs->start_sector - 1);
  inode->eof_offset_byte =
    BFSVAL32 ((f->start - bfs->start_sector) * DEV_BSIZE + f->size - 1);

  DPRINTF (f->bfs->debug, "%s: start %d end %d\n", __func__,
	   BFSVAL32 (inode->start_sector), BFSVAL32 (inode->end_sector));

  if (!bfs_writeback_inode (bfs, inode))
    return EIO;

  return 0;
}

int
bfs_datafile_write (struct bfs_datafile *f, uint8_t *buf)
{
  struct bfs *bfs = f->bfs;

  if ((f->current + 1) * DEV_BSIZE >= bfs->data_end)
    {
      DPRINTF (bfs->debug, "disk full.\n");
      return ENOSPC;
    }

  DPRINTF (bfs->debug, "%s: sector %d\n", __FUNCTION__, f->current);

  if (!bfs->io->write (bfs->io->cookie, buf, f->current))
    {
      return EIO;
    }

  f->size += DEV_BSIZE;
  f->current++;
  f->size_changed = TRUE;

  return 0;
}

int
bfs_datafile_read (struct bfs_datafile *f, uint8_t *buf, uint32_t n)
{
  struct bfs *bfs = f->bfs;

  if ((f->start + n) * DEV_BSIZE >= bfs->data_end)
    {
      DPRINTF (bfs->debug, "disk overrun.\n");
      return ENOSPC;
    }

  if (!bfs->io->read (bfs->io->cookie, buf, f->start + n))
    {
      return EIO;
    }

  return 0;
}
