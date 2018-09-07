/*	$NetBSD: bfs.h,v 1.5 2008/04/28 20:24:02 martin Exp $	*/

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

#ifndef _FS_SYSVBFS_BFS_H_
#define	_FS_SYSVBFS_BFS_H_
#include <sys/block_io.h>
/*
 *   Boot File System
 *
 *	+----------
 *	|bfs_super_block (512byte)
 *	|				1 sector
 *	|
 *	+----------
 *	|bfs_inode (64byte) * 8
 *	|    .				1 sector
 *	|bfs_inode
 *	+----------  <--- bfs_super_block.header.data_start
 *	|DATA BLOCK
 *	|    .       Root (/) dirent.   1 sector
 *      |----------
 *	|    .
 *	|
 *	+----------  <--- bfs_super_block.header.data_end
 */

/* File system global */
#define	BFS_SUPERBLOCK_SIZE	1024	// superblock + ilist
#define	BFS_DIRENT_SIZE		512	// root dirent size.

#define	DEV_BSIZE	512
#define	DEV_BSHIFT	9		/* log2(DEV_BSIZE) */


/* BFS specification */
#define	BFS_SECTOR		0	/* no offset */
#define	BFS_MAGIC		0x1badface
#define	BFS_FILENAME_MAXLEN	14
#define	BFS_ROOT_INODE		2
#define	BFS_BSIZE		512
#define	BFS_BSHIFT		9

typedef int32_t		sysvbfs_uid_t;
typedef uint32_t	sysvbfs_mode_t;

struct bfs_super_block_header
{
  uint32_t magic;
  uint32_t data_start_byte;
  uint32_t data_end_byte;
} __attribute ((packed));

struct bfs_compaction
{
  uint32_t from;
  uint32_t to;
  uint32_t from_backup;
  uint32_t to_backup;
} __attribute ((packed));

struct bfs_fileattr
{
  uint32_t type;
#define	BFS_FILE_TYPE_DIR	2
#define	BFS_FILE_TYPE_FILE	1
  uint32_t mode;
  int32_t uid;
  int32_t gid;
  uint32_t nlink;
  int32_t atime;
  int32_t mtime;
  int32_t ctime;
  int32_t padding[4];
} __attribute ((packed));	/* 48byte */

struct bfs_inode
{
  uint16_t number;		/*  0 */
  int16_t padding;
  uint32_t start_sector;		/*  4 */
  uint32_t end_sector;		/*  8 */
  uint32_t eof_offset_byte;	/* 12 (offset from super block start) */
  struct bfs_fileattr attr;	/* 16 */
} __attribute ((packed));	/* 64byte */

struct bfs_super_block
{
  struct bfs_super_block_header header;
  struct bfs_compaction compaction;
  char fsname[6];
  char volume[6];
  int32_t padding[118];
} __attribute ((packed));

struct bfs_dirent
{
  uint16_t inode;
  char name[BFS_FILENAME_MAXLEN];
} __attribute ((packed)); /* 16byte */

/* Software definition */
struct bfs
{
  int32_t start_sector;
  /* Super block */
  struct bfs_super_block *super_block;
  size_t super_block_size;

  /* Data block */
  uint32_t data_start, data_end;

  /* Inode */
  struct bfs_inode *inode;
  uint32_t n_inode;
  uint32_t max_inode;

  /* root directory */
  struct bfs_dirent *dirent;
  size_t dirent_size;
  uint32_t n_dirent;
  uint32_t max_dirent;
  struct bfs_inode *root_inode;

  /* Sector I/O operation */
  const struct block_io_ops *io;

  bool debug;
};

struct bfs_init_arg
{
  struct bfs *bfs;
  const struct block_io_ops *io_ops;
  daddr_t start_sector;
  size_t superblock_size;
  size_t dirent_size;
  uint8_t *superblock;
  uint8_t *dirent;
  bool debug;
};
__BEGIN_DECLS
// Initialize all.
int bfs_init (struct bfs_init_arg *);

// Initialze utility functions.
int bfs_init_superblock (struct bfs *, uint32_t, size_t *);
int bfs_init_inode (struct bfs *, uint8_t *, size_t *);
int bfs_init_dirent (struct bfs *, uint8_t *);

// BFS access ops.
int bfs_file_read (const struct bfs *, const char *, void *, size_t, size_t *);
int bfs_file_write (struct bfs *, const char *, void *, size_t);
int bfs_file_create (struct bfs *, const char *, void *,  size_t,
		     const struct bfs_fileattr *);
int bfs_file_delete (struct bfs *, const char *);
int bfs_file_rename (struct bfs *, const char *, const char *);
bool bfs_file_lookup (const struct bfs *, const char *, uint32_t *, uint32_t *,
		      size_t *);
size_t bfs_file_size (const struct bfs_inode *);
#ifdef BFS_DEBUG
bool bfs_dump (const struct bfs *);
#endif

/* filesystem ops */
struct vnode;
bool bfs_inode_lookup (const struct bfs *, ino_t, struct bfs_inode **);
bool bfs_dirent_lookup_by_name (const struct bfs *, const char *,
				struct bfs_dirent **);
bool bfs_dirent_lookup_by_inode (const struct bfs *, uint32_t,
				 struct bfs_dirent **);
void bfs_inode_set_attr (const struct bfs *, struct bfs_inode *,
			 const struct bfs_fileattr *);
int bfs_inode_alloc (const struct bfs *, struct bfs_inode **, uint32_t *,
		     uint32_t *);

/* super block ops. */
bool bfs_superblock_valid (const struct bfs_super_block *);
bool bfs_writeback_dirent (const struct bfs *, struct bfs_dirent *, bool);
bool bfs_writeback_inode (const struct bfs *, struct bfs_inode *);

// Misc util.
struct disklabel;
bool sysvbfs_partition_find (struct disklabel *, uint32_t *);

__END_DECLS
#define	ROUND_SECTOR(x)		 (((x) + 511) & ~511)
#define	TRUNC_SECTOR(x)		((x) & ~511)

#if BYTE_ORDER == BIG_ENDIAN
#define	BFSVAL16(x)	bswap16(x)
#define	BFSVAL32(x)	bswap32(x)
#else
#define	BFSVAL16(x)	(x)
#define	BFSVAL32(x)	(x)
#endif

#endif /* _FS_SYSVBFS_BFS_H_ */
