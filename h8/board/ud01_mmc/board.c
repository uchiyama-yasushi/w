
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
#include <sys/delay.h>
#include <sys/shell.h>

#include <reg.h>
#include <sys/board.h>

#include <stdlib.h>
#include <string.h>
#include <sys/mmc.h>
#include <sysvbfs/bfs.h>
#include <pcat_mbr.h>
#include <sys/disklabel_netbsd.h>

SHELL_COMMAND_DECL (res);
SHELL_COMMAND_DECL (ini);
SHELL_COMMAND_DECL (csd);
SHELL_COMMAND_DECL (ocr);
SHELL_COMMAND_DECL (cid);
SHELL_COMMAND_DECL (read);
SHELL_COMMAND_DECL (write);

bool mbr_partition_info (struct mbr_partition *, int);
#define	BFS_SUPERBLOCK_SIZE	1024
#define	BFS_DIRENT_SIZE		512
STATIC uint8_t bfs_superblock[BFS_SUPERBLOCK_SIZE];
STATIC uint8_t bfs_dirent[BFS_DIRENT_SIZE];

uint8_t buf[1024];

void
board_main (uint32_t arg __attribute__((unused)))
{
  shell_command_register (&res_cmd);
  shell_command_register (&ini_cmd);
  shell_command_register (&csd_cmd);
  shell_command_register (&ocr_cmd);
  shell_command_register (&cid_cmd);
  shell_command_register (&read_cmd);
  shell_command_register (&write_cmd);

  printf ("-----start------\n");
  static struct block_io_ops io;

  mmc_init (&io);
  io.drive (io.cookie, 0);

  io.read (io.cookie, buf, 0);
  struct mbr_partition *partition_table = mbr_partition_table (buf);
  if (partition_table)
    mbr_partition_foreach (partition_table, mbr_partition_info);
  int i, n;
  for (i = 0; i < 4; i++)
    if (partition_table[i].partition_id == 169)	// NetBSD
      break;
  if (i == 4)
    return;
  io.read (io.cookie, buf,
	    MBRVAL32 (partition_table[i].lba_start_sector) + LABEL_SECTOR);

  const struct disklabel *d = (const struct disklabel *)buf;
  if (MBRVAL32 (d->d_magic) != DISKMAGIC)
    {
      printf ("no disklabel NetBSD partition.\n");
      return;
    }
  const struct partition *p = d->d_partitions;
  n = MBRVAL16 (d->d_npartitions);
  for (i = 0; i < n; i++, p++)
    if (p->p_fstype == 25)	// SysVBFS
      break;
  if (i == n)
    {
      printf ("no SysVBFS partition.\n");
      return;
    }
  uint32_t bfs_start_sector = MBRVAL32 (p->p_offset);
  printf ("SysVBFS sector = %D size = %D\n", n, MBRVAL32 (p->p_size));

  struct bfs_init_arg init_arg;
  struct bfs bfs;
  struct bfs *b;

  init_arg.start_sector = bfs_start_sector;
  init_arg.superblock_size = BFS_SUPERBLOCK_SIZE;
  init_arg.superblock = bfs_superblock;
  init_arg.dirent_size = BFS_DIRENT_SIZE;
  init_arg.dirent = bfs_dirent;
  init_arg.io_ops = &io;
  init_arg.debug = TRUE;
  init_arg.bfs = &bfs;

  if ((b = bfs_init (&init_arg)) == NULL)
    {
      printf ("bfs failed.\n");
      return;
    }
  printf ("a1\n");
  int err;
  size_t readed;

  if ((err = bfs_file_read (b, "test.txt", buf, sizeof buf, &readed)) != 0)
    {
      printf ("%s: read failed. (%d)\n", "test.txt", err);
      return;
    }
  printf ("a2\n");

  for (i = 0; i < (int)readed; i++)
    printf ("%c", buf[i]);

  if ((err = bfs_file_write (b, "a.txt", buf, readed)) != 0)
    {
      printf ("%d: write failed.\n", err);
    }
}


uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | BUFFERED_CONSOLE_ENABLE | DELAY_CALIBRATE;
}


uint32_t
res (int32_t argc __attribute__((unused)),
	   const char *argv[] __attribute__((unused)))
{
  mmc_cmd_reset ();

  return 0;
}

uint32_t
ini (int32_t argc __attribute__((unused)),
	  const char *argv[] __attribute__((unused)))
{
  mmc_cmd_init ();

  return 0;
}

uint32_t
ocr (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
  mmc_cmd_ocr ();

  return 0;
}

uint32_t
csd (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
  mmc_cmd_csd ();

  return 0;
}

uint32_t
cid (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
  mmc_cmd_cid ();

  return 0;
}

uint8_t readbuf[512];
uint32_t
read (int32_t argc __attribute__((unused)),
      const char *argv[] __attribute__((unused)))
{
  if (argc < 2)
    return 0;

  uint32_t lba = atoi (argv[1]);
  iprintf ("sector %d\n", lba);

  mmc_read (NULL, readbuf, lba);
  int i, j, k;
  for (j = k = 0; j < 16; j++)
    {
      for (i = 0; i < 32; i++, k++)
	{
	  iprintf ("%x ", (uint32_t)readbuf[k] & 0xff);
	}
      fputc ('\n', altout);
    }

  return 0;
}

uint32_t
write (int32_t argc __attribute__((unused)),
       const char *argv[] __attribute__((unused)))
{
  if (argc < 3)
    return 0;

  uint32_t lba = atoi (argv[1]);
  iprintf ("sector %d\n", lba);
  memset (readbuf, atoi (argv[2]), 512);

  mmc_write (NULL, readbuf, lba);

  return 0;
}

