
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
#include <sys/shell.h>
#include <pcat/bfs_bios.h>
#include <stdlib.h>
#include <string.h>

SHELL_COMMAND_DECL (info);
SHELL_COMMAND_DECL (backup);
SHELL_COMMAND_DECL (install);

STATIC int do_backup (struct block_io_ops *);
STATIC struct bfs *booted_disk_bfs;
STATIC int __boot_drive;
STATIC struct bfs_fileattr default_file_attr;
#define	BACKUP_SECTOR	64
#define	BACKUP_FILENAME	"backup.img"
STATIC uint8_t backup_buffer[BACKUP_SECTOR * DEV_BSIZE];
#define	BOOT1_SECTOR	1
#define	BOOT2_SECTOR	48
STATIC uint8_t boot1_buffer[BOOT1_SECTOR * DEV_BSIZE];
STATIC uint8_t boot2_buffer[BOOT2_SECTOR * DEV_BSIZE];

__BEGIN_DECLS
void installer_init (int);
__END_DECLS

void
installer_init (int boot_drive)
{
  struct bfs *(*bfs_init_func)(int);

  __boot_drive = boot_drive;
  bfs_init_func = boot_drive & 0x80 ? disk_bfs_init : floppy_bfs_init;

  if ((booted_disk_bfs = bfs_init_func (boot_drive)) == NULL)
    {
      printf ("*** CANNOT ACCESS BOOTED DRIVE ***\n");
      return;
    }

  default_file_attr.mode = 0x1a4;	/* chmod 644 */

  shell_command_register (&info_cmd);
  shell_command_register (&backup_cmd);
  shell_command_register (&install_cmd);
}

uint32_t
info (int32_t argc, const char *argv[])
{
  struct bfs_dirent *file;
  struct bfs_inode *inode;
  int i, drive;

  drive = -1;
  if (argc > 1)
    drive = atoi (argv[1]) & 0x3;

  print_disk_info (drive);

  if (drive >= 0)
    return 0;

  printf ("\n[Booted Disk 0x%x (%s)]\n", __boot_drive,
	  __boot_drive & 0x80 ? "Hard Disk" : "Floppy");
  for (file = booted_disk_bfs->dirent, i = 0;i < booted_disk_bfs->max_dirent;
       i++, file++)
    {
      if (file->inode != 0)
	{
	  inode = &booted_disk_bfs->inode[file->inode - BFS_ROOT_INODE];
	  if (inode->attr.type != BFS_FILE_TYPE_FILE)
	    continue;

	  printf ("%s %d bytes (%d-%d)sector\n", file->name,
		  bfs_file_size(inode), inode->start_sector,
		  inode->end_sector);
	}
    }

  return 0;
}

uint32_t
backup (int32_t argc, const char *argv[])
{
  struct block_io_ops *io;
  uint8_t drive;

  if (argc < 2)
    drive = 0x80;
  else
    drive = atoi (argv[1]) | 0x80;

  if ((io = disk_init (drive)) == NULL)
    {
      printf ("ERROR: Couldn't access disk 0x%x.\n", drive);
      return 1;
    }

  printf ("Backup disk 0x%x\n", drive);
  if (do_backup (io) == 0)
    printf ("done.\n");
  else
    return 1;

  return 0;
}

uint32_t
install (int32_t argc, const char *argv[])
{
  struct mbr_partition *partition;
  struct block_io_ops *io;
  uint8_t drive;
  size_t readed;
  int err;

  if (argc < 2)
    drive = 0x80;	// Hard disk 0
  else
    drive = atoi (argv[1]) | 0x80;	// Hard disk.

  // Read MBR.
  if ((io = disk_init (drive)) == NULL || !io->read (io->cookie,
						     boot1_buffer, 0))
    {
      printf ("ERROR: Couldn't access disk 0x%x.\n", drive);
      return 1;
    }
  if ((partition = mbr_partition_table (boot1_buffer)) == NULL)
    return 1;

  // Check boot2 region.
  if (partition[0].lba_start_sector < BOOT1_SECTOR + BOOT2_SECTOR)
    {
      printf ("ERROR: Not enuf region for boot2.\n");
      return 1;
    }

  printf ("Install bootloader to disk 0x%x\n", drive);

  if ((err = bfs_file_read (booted_disk_bfs, "boot1", boot1_buffer,
			    sizeof boot1_buffer, &readed)) != 0)
    {
      printf ("Couldn't read boot1.\n");
      return 1;
    }

  if ((err = bfs_file_read (booted_disk_bfs, "boot2", boot2_buffer,
			    sizeof boot2_buffer, &readed)) != 0)
    {
      printf ("Couldn't read boot1.\n");
      return 1;
    }

  // Backup original sector image.
  if (do_backup (io) != 0)
    return 1;

  memcpy (backup_buffer, boot1_buffer, 11);
  memcpy (backup_buffer + 62, boot1_buffer + 62, 156);
  //  memcpy (backup_buffer + 510, boot1_buffer + 510, 2);
  memcpy (backup_buffer + 512, boot2_buffer, readed);

  // Write back.
  if (!io->write_n (io->cookie, backup_buffer, 0, BACKUP_SECTOR))
    {
      printf ("Write back failed.\n");
      return 1;
    }

  printf ("done.\n");
  return 0;
}

int
do_backup (struct block_io_ops *io)
{
  int err;

  if (!io->read_n (io->cookie, backup_buffer, 0, BACKUP_SECTOR))
    {
      printf ("Backup failed. (read)\n");
      return 2;
    }

  bfs_file_delete (booted_disk_bfs, BACKUP_FILENAME);

  if ((err = bfs_file_create (booted_disk_bfs, BACKUP_FILENAME, backup_buffer,
			      sizeof backup_buffer, &default_file_attr)) != 0)
    {
      printf ("Backup failed. (write) cause=%d\n", err);
    }

  return 0;
}
