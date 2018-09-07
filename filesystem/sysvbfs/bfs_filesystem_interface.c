/*	$NetBSD: bootfs.c,v 1.4 2008/04/28 20:23:18 martin Exp $	*/

/*-
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
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

/* System V bfs */
typedef uint32_t off_t;
struct stat;
struct fs_ops;

struct __file
{
  void *fs_ctx;
  struct fs_ops *ops;
};

struct fs_ops
{
  int (*open) (struct __file *, const char *);
  int (*close) (struct __file *);
  int (*read) (struct __file *, void *, size_t, size_t *);
  int (*write) (struct __file *, void *, size_t, size_t *);
  off_t (*seek) (struct __file *, off_t, int);
  int (*stat) (struct __file *, struct stat *);
};

int bfs_init (struct bfs **);
int bfs_open (struct __file *, const char *);
int bfs_close (struct __file *);
int bfs_read(struct __file *, void *, size_t, size_t *);
int bfs_write(struct __file *, void *, size_t, size_t *);
off_t bfs_seek(struct __file *, off_t, int);
int bfs_stat(struct __file *, struct stat *);

void *alloc (size_t);
void dealloc (void *, size_t);

struct fs_ops bfs_ops =
  {
    bfs_open, bfs_close, bfs_read, bfs_write, bfs_seek, bfs_stat
  };

struct bfs_file
{
  struct bfs *bfs;
  int start, end;
  size_t size;
  int cur;
  uint8_t buf[DEV_BSIZE];
};

int
bfs_open (struct __file *f, const char *name)
{
  struct bfs_file *file;

  if ((file = alloc (sizeof *file)) == 0)
    return -1;
  memset (file, 0, sizeof *file);

  if (bfs_init (&file->bfs) != 0)
    {
      dealloc (file, sizeof *file);
      return -2;
    }

  if (!bfs_file_lookup (file->bfs, name, &file->start, &file->end,
		       &file->size))
    {
      bfs_fini (file->bfs);
      dealloc (file, sizeof *file);
      return -3;
    }

  printf ("%s: %s %d %d %d\n", __func__, name, file->start,
	 file->end, file->size);

  f->fs_ctx = file;

  return 0;
}

int
bfs_close (struct __file *f)
{
  struct bfs_file *file = f->fs_ctx;

  bfs_fini (file->bfs);

  return 0;
}

int
bfs_read (struct __file *f, void *buf, size_t size, size_t *resid)
{
  struct bfs_file *file = f->fs_ctx;
  int n, start, end;
  uint8_t *p = buf;
  size_t bufsz = size;
  int cur = file->cur;

  if (cur + size > file->size)
    size = file->size - cur;

  start = file->start + (cur >> DEV_BSHIFT);
  end = file->start + ((cur + size) >> DEV_BSHIFT);

  /* first sector */
  if (!sector_read (0, file->buf, start))
    return -2;
  n = TRUNC_SECTOR (cur) + DEV_BSIZE - cur;
  if (n >= size)
    {
      memcpy (p, file->buf + DEV_BSIZE - n, size);
      goto done;
    }
  memcpy (p, file->buf + DEV_BSIZE - n, n);
  p += n;

  if ((end - start - 1) > 0)
    {
      if (!sector_read_n (0, p, start + 1, end - start - 1))
	return -2;
      p += (end - start - 1) * DEV_BSIZE;
    }

  /* last sector */
  if (!sector_read (0, file->buf, end))
    return -2;
  n = cur + size - TRUNC_SECTOR (cur + size);
  memcpy (p, file->buf, n);

 done:
  file->cur += size;

  if (resid)
    *resid = bufsz - size;

  return 0;
}

int
bfs_write (struct __file *f, void *start, size_t size, size_t *resid)
{

  return -1;
}
