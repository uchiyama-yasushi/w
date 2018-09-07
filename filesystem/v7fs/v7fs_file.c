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

static int lookup_subr (struct v7fs_self *, void *, v7fs_daddr_t, size_t);
static int remove_subr (struct v7fs_self *, void *, v7fs_daddr_t, size_t);

int
v7fs_file_lookup_by_name (struct v7fs_self *_, struct v7fs_inode *parent_dir,
			  const char *name, v7fs_ino_t *ino)
{
  struct v7fs_lookup_arg lookup_arg;
  char filename[V7FS_NAME_MAX];
  char *q;
  int error;

  if ((q = strchr (name, '/')))
    {
      memcpy (filename, name, q - name);
      filename[q - name] = '\0';
    }
  else
    {
      strncpy (filename, name, V7FS_NAME_MAX);
    }
  DPRINTF ("%s dir=%d\n", name, parent_dir->inode_number);
  lookup_arg.name = filename;
  lookup_arg.inode_number = 0;

  if ((error = v7fs_datablock_foreach (_, parent_dir, lookup_subr, &lookup_arg))
      != V7FS_ITERATOR_BREAK)
    return ENOENT;

  *ino = lookup_arg.inode_number;

  return 0;
}

int
lookup_subr (struct v7fs_self *_, void *ctx, v7fs_daddr_t blk, size_t sz)
{
  struct v7fs_lookup_arg *p = (struct v7fs_lookup_arg *)ctx;
  struct v7fs_dirent *dir;
  const char *name = p->name;
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
      if (dir->inode_number < 1)
	{
	  DPRINTF ("*** bad inode #%d ***\n", dir->inode_number);
	  continue;
	}

      if (strncmp ((const char *)dir->name, name, V7FS_NAME_MAX) == 0)
	{
	  //	  DPRINTF ("FOUND %s %d\n", dir->name, dir->inode_number);
	  p->inode_number = dir->inode_number;
	  ret =  V7FS_ITERATOR_BREAK; // found
	  break;
	}
    }
  scratch_free (_, buf);

  return ret;
}

int
v7fs_file_allocate (struct v7fs_self *_, struct v7fs_inode *parent_dir,
		    const char *name, struct v7fs_fileattr *attr,
		    v7fs_ino_t *ino)
{
  struct v7fs_inode inode;
  int error;

  DPRINTF ("\n");
  // Check filename.
  if (v7fs_file_lookup_by_name (_, parent_dir, name, ino) == 0)
    {
      DPRINTF ("%s exists\n", name);
      return EEXIST;
    }

  // Get new inode.
  if ((error = v7fs_inode_allocate (_, ino)))
    return error;
  // Set initial attribute.
  memset (&inode, 0, sizeof (struct v7fs_inode));
  inode.inode_number = *ino;
  inode.mode = attr->mode;
  inode.uid = attr->uid;
  inode.gid = attr->gid;
  if (attr->ctime)
    inode.ctime = attr->ctime;
  if (attr->mtime)
    inode.mtime = attr->mtime;
  if (attr->atime)
    inode.atime = attr->atime;

  switch (inode.mode & V7FS_IFMT)
    {
    default:
      DPRINTF ("Can't allocate %o type.\n", inode.mode);
      v7fs_inode_deallocate (_, *ino);
      printf ("sanachan %s\n", __func__);
      return EINVAL;
    case V7FS_IFCHR:
      // FALLTHROUGH
    case V7FS_IFBLK:
      inode.nlink = 1;
      inode.device = attr->device;
      inode.addr[0] = inode.device;
      break;
    case V7FSBSD_IFSOCK:
      // FALLTHROUGH
    case V7FSBSD_IFLNK:
      // FALLTHROUGH
    case V7FS_IFREG:
      inode.nlink = 1;
      break;
    case V7FS_IFDIR:
      inode.nlink = 2;	// . + ..
      if ((error = v7fs_datablock_expand (_, &inode,
					  sizeof (struct v7fs_dirent) * 2)))
	{
	  v7fs_inode_deallocate (_, *ino);
	  return error;
	}
      v7fs_daddr_t blk = inode.addr[0];
      void *buf;
      if (!(buf = scratch_read (_, blk)))
	{
	  v7fs_inode_deallocate (_, *ino);
	  return EIO;
	}
      struct v7fs_dirent *dir = (struct v7fs_dirent *)buf;
      strcpy (dir[0].name, ".");
      dir[0].inode_number = V7FS_VAL16 (_, *ino);
      strcpy (dir[1].name, "..");
      dir[1].inode_number = V7FS_VAL16 (_, parent_dir->inode_number);
      if (!_->io.write (_->io.cookie, buf, blk))
	{
	  scratch_free (_, buf);
	  return EIO;
	}
      scratch_free (_, buf);
      break;
    }

  v7fs_inode_writeback (_, &inode);

  // Link this inode to parent directory.
  if ((error = v7fs_directory_add_entry (_, parent_dir, *ino, name)))
    {
      DPRINTF ("can't add dirent.\n");
      return error;
    }

  return 0;
}

int
v7fs_file_deallocate (struct v7fs_self *_, struct v7fs_inode *parent_dir,
		      const char *name)
{
  v7fs_ino_t ino;
  struct v7fs_inode inode;
  int error;

  DPRINTF ("%s\n", name);
  if ((error = v7fs_file_lookup_by_name (_, parent_dir, name, &ino)) != 0)
    {
      printf ("no such a file: %s\n", name);
      return error;
    }
  DPRINTF ("%s->#%d\n", name, ino);
  if ((error = v7fs_inode_load (_, &inode, ino)))
    return error;

  if (v7fs_inode_isdir (&inode))
    {
      // Check child directory exists.
      if (v7fs_inode_filesize (&inode) !=
	  sizeof (struct v7fs_dirent) * 2 /*"." + ".."*/)
	{
	  DPRINTF ("file exists.\n");
	  return EEXIST;
	}
      inode.nlink = 0;	// remove this.
    }
  else
    {
      // Decrement reference count.
      --inode.nlink;	// regular file.
    }


  if ((error = v7fs_directory_remove_entry (_, parent_dir, name)))
    return error;
  DPRINTF ("remove dirent\n");

  if (inode.nlink == 0)
    {
      v7fs_datablock_contract (_, &inode, inode.filesize);
      DPRINTF ("remove datablock\n");
      v7fs_inode_deallocate (_, ino);
      DPRINTF ("remove inode\n");
    }
  else
    {
      v7fs_inode_writeback (_, &inode);
    }

  return 0;
}

int
v7fs_directory_add_entry (struct v7fs_self *_, struct v7fs_inode *parent_dir,
			  v7fs_ino_t ino, const char *name)
{
  struct v7fs_inode inode;
  struct v7fs_dirent *dir;
  int error = 0;
  v7fs_daddr_t blk;
  void *buf;

  DPRINTF ("%s %d\n", name, ino);
  // Target inode
  if ((error = v7fs_inode_load (_, &inode, ino)))
    return error;

  // Expand datablock.
  if ((error = v7fs_datablock_expand (_, parent_dir, sizeof (struct v7fs_dirent))))
    return error;

  // Read last entry.
  if (!(blk = v7fs_datablock_last (_, parent_dir,
	      v7fs_inode_filesize (parent_dir))))
    return EIO;

  // Load dirent block. (This vnode is locked by VFS layer ??? -uch)
  if (!(buf = scratch_read (_, blk)))
    return EIO;

  size_t sz = v7fs_inode_filesize (parent_dir);
  sz = V7FS_RESIDUE_BSIZE (sz);	// last block payload.
  int n = sz / sizeof (struct v7fs_dirent) - 1;
  // Add dirent.
  dir = (struct v7fs_dirent *)buf;
  dir[n].inode_number = V7FS_VAL16 (_, ino);
  strncpy ((char *)dir[n].name, name, V7FS_NAME_MAX);
  // Write back datablock
  if (!_->io.write (_->io.cookie, buf, blk))
    error = EIO;
  scratch_free (_, buf);
  //
  if (v7fs_inode_isdir (&inode))
    {
      parent_dir->nlink++;
      v7fs_inode_writeback (_, parent_dir);
    }

  DPRINTF ("done. (dirent size=%dbyte)\n", parent_dir->filesize);

  return error;
}

int
v7fs_directory_remove_entry (struct v7fs_self *_, struct v7fs_inode *parent_dir,
			     const char *name)
{
  struct v7fs_lookup_arg lookup_arg;
  struct v7fs_inode inode;
  int error;
  struct v7fs_dirent lastdirent;
  v7fs_daddr_t lastblk;
  size_t sz, lastsz;
  v7fs_off_t pos;
  void *buf;

  // Setup replaced entry.
  sz = parent_dir->filesize;
  lastblk = v7fs_datablock_last (_, parent_dir, v7fs_inode_filesize (parent_dir));
  lastsz = V7FS_RESIDUE_BSIZE (sz);
  pos = lastsz - sizeof (struct v7fs_dirent);

  if (!(buf = scratch_read (_, lastblk)))
    return EIO;
  lastdirent = *((struct v7fs_dirent *)((uint8_t *)buf + pos));
  scratch_free (_, buf);
  DPRINTF ("last dirent=%d %s pos=%d\n",
	   V7FS_VAL16 (_, lastdirent.inode_number),
	   lastdirent.name, pos);

  lookup_arg.name = name;
  lookup_arg.replace = &lastdirent; //disk endian

  // Search entry that removed. replace it to last dirent.
  if ((error = v7fs_datablock_foreach (_, parent_dir, remove_subr,
				       &lookup_arg))
      != V7FS_ITERATOR_BREAK)
    return ENOENT;

  // Contract dirent entries.
  v7fs_datablock_contract (_, parent_dir, sizeof (struct v7fs_dirent));
  DPRINTF ("done. (dirent size=%dbyte)\n", parent_dir->filesize);

  // Target inode
  if ((error = v7fs_inode_load (_, &inode, lookup_arg.inode_number)))
    return error;

  if (v7fs_inode_isdir (&inode))
    {
      parent_dir->nlink--;
      v7fs_inode_writeback (_, parent_dir);
    }

  return 0;
}

int
remove_subr (struct v7fs_self *_, void *ctx, v7fs_daddr_t blk, size_t sz)
{
  struct v7fs_lookup_arg *p = (struct v7fs_lookup_arg *)ctx;
  struct v7fs_dirent *dir;
  void *buf;
  size_t i;
  int ret = 0;

  DPRINTF ("match start blk=%x\n", blk);
  if (!(buf = scratch_read (_, blk)))
    return EIO;

  dir = (struct v7fs_dirent *)buf;

  for (i = 0; i < sz / sizeof (struct v7fs_dirent); i++, dir++)
    {
      DPRINTF ("%d\n", V7FS_VAL16 (_, dir->inode_number));
      if (strncmp (p->name, (const char *)dir->name, V7FS_NAME_MAX) == 0)
	{
	  p->inode_number = V7FS_VAL16 (_, dir->inode_number);
	  // Replace to last dirent.
	  *dir = *(p->replace); // disk endian
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
