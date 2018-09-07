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
#include "v7fs_dirent.h"
#include "v7fs_file.h"
#include "v7fs_datablock.h"

#ifdef V7FS_FILE_DEBUG
#define	DPRINTF(fmt, args...)	printf("%s: " fmt, __func__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

static int replace_subr (struct v7fs_self *, void *, v7fs_daddr_t, size_t);
static int lookup_by_number_subr (struct v7fs_self *, void *, v7fs_daddr_t,
				  size_t);

int
v7fs_file_link (struct v7fs_self *_, struct v7fs_inode *parent_dir,
		struct v7fs_inode *p, const char *name)
{
  int error = 0;

  DPRINTF ("%d %d %s\n", parent_dir->inode_number, p->inode_number, name);
  if ((error = v7fs_directory_add_entry (_, parent_dir, p->inode_number, name)))
    {
      DPRINTF ("can't add entry");
      return error;
    }
  p->nlink++;
  v7fs_inode_writeback (_, p);

  return 0;
}

int
v7fs_file_rename (struct v7fs_self *_, struct v7fs_inode *parent_from,
		  const char *from, struct v7fs_inode *parent_to, const char *to)
{
  v7fs_ino_t from_ino, to_ino;
  int error;

  if ((error = v7fs_file_lookup_by_name (_, parent_from, from, &from_ino)))
    {
      DPRINTF ("%s don't exists\n", from);
      return error;
    }

  if ((error = v7fs_file_lookup_by_name (_, parent_to, to, &to_ino)) != ENOENT)
    {
      DPRINTF ("%s already exists\n", to);
      return EEXIST;
    }

  if ((error = v7fs_directory_add_entry (_, parent_to, from_ino, to)))
    {
      DPRINTF ("can't add entry");
      return error;
    }
  if ((error = v7fs_directory_remove_entry (_, parent_from, from)))
    {
      DPRINTF ("can't remove entry");
      return error;
    }

  if (parent_from != parent_to)
    {
      // If directory remove, update ".."
      struct v7fs_inode inode;
      v7fs_inode_load (_, &inode, from_ino);
      if (v7fs_inode_isdir (&inode))
	{
	  if ((error = v7fs_directory_replace_entry (_, &inode, "..",
						     parent_to->inode_number)))
	    {
	      DPRINTF ("can't replace parent dir");
	      return error;
	    }
	  v7fs_inode_writeback (_, &inode);
	}
    }

  return 0;
}


int
v7fs_directory_replace_entry (struct v7fs_self *_,
			      struct v7fs_inode *self_dir,
			      const char *name, v7fs_ino_t ino)
{
  struct v7fs_lookup_arg lookup_arg;
  int error;

  lookup_arg.name = name;
  lookup_arg.inode_number = ino;
  // Search entry that replaced. replace it to new inode number.
  if ((error = v7fs_datablock_foreach (_, self_dir, replace_subr, &lookup_arg))
      != V7FS_ITERATOR_BREAK)
    return ENOENT;

  return 0;
}

int
replace_subr (struct v7fs_self *_, void *ctx, v7fs_daddr_t blk, size_t sz)
{
  struct v7fs_lookup_arg *p = (struct v7fs_lookup_arg *)ctx;
  struct v7fs_dirent *dir;
  void *buf;
  size_t i, n;
  int ret = 0;

  DPRINTF ("match start blk=%x\n", blk);
  if (!(buf = scratch_read (_, blk)))
    return EIO;

  dir = (struct v7fs_dirent *)buf;
  n = sz / sizeof (struct v7fs_dirent);

  for (i = 0; i < n; i++, dir++) //disk endian
    {
      if (strncmp (p->name, (const char *)dir->name, V7FS_NAME_MAX) == 0)
	{
	  // Replace inode#
	  dir->inode_number = V7FS_VAL16 (_, p->inode_number);
	  // Write back.
	  if (!_->io.write (_->io.cookie, buf, blk))
	    ret = EIO;
	  else
	    ret = V7FS_ITERATOR_BREAK;
	  break;
	}
    }
  scratch_free (_, buf);

  return ret;
}

bool
v7fs_file_lookup_by_number (struct v7fs_self *_,
			    struct v7fs_inode *parent_dir,
			    v7fs_ino_t ino, char *buf)
{
  struct v7fs_lookup_arg lookup_arg;
  int ret;

  lookup_arg.inode_number = ino;
  lookup_arg.buf = buf;

  ret = v7fs_datablock_foreach (_, parent_dir,
				lookup_by_number_subr, &lookup_arg);

  return ret == V7FS_ITERATOR_BREAK;
}

int
lookup_by_number_subr (struct v7fs_self *_, void *ctx, v7fs_daddr_t blk, size_t sz)
{
  struct v7fs_lookup_arg *p = (struct v7fs_lookup_arg *)ctx;
  struct v7fs_dirent *dir;
  void *buf;
  size_t i, n;
  int ret = 0;

  //  DPRINTF ("match start blk=%x\n", blk);
  if (!(buf = scratch_read (_, blk)))
    return EIO;

  dir = (struct v7fs_dirent *)buf;
  n = sz / sizeof (struct v7fs_dirent);
  v7fs_dirent_endian_convert (_, dir, n);

  for (i = 0; i < n; i++, dir++)
    {
      if (dir->inode_number == p->inode_number)
	{
	  if (p->buf)
	    strncpy (p->buf, dir->name, V7FS_NAME_MAX);
//	  DPRINTF ("FOUND %s %d\n", dir->name, dir->inode_number);
	  ret = V7FS_ITERATOR_BREAK;
	  break;
	}
    }
  scratch_free (_, buf);

  return ret;
}
