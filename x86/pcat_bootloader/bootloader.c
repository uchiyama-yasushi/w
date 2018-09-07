
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
#include <sys/types.h>
#include <sys/board.h>

#include <sys/console.h>
#include <sys/shell.h>

#include <pcat_mbr.h>
#include <sysvbfs/bfs.h>

#include <stdlib.h>
#include <string.h>

#include <pcat/bios.h>
#include "bootloader.h"
#include "mbr.h"
#include "disklabel.h"
#include "sysvbfs.h"

#include <sys/timer.h>

SHELL_COMMAND_DECL (chain);
SHELL_COMMAND_DECL (load);
SHELL_COMMAND_DECL (active);
SHELL_COMMAND_DECL (id);
SHELL_COMMAND_DECL (info);
SHELL_COMMAND_DECL (autoboot);
SHELL_COMMAND_DECL (autoactive);
SHELL_COMMAND_DECL (console);

extern struct bootloader bootloader_saved;
struct bootloader bootloader;
STATIC bool __config_save (bool);
STATIC bool __config_index (int *, int32_t, const char *[]);
STATIC bool check_autoboot (void);
STATIC void do_autoboot (void);

#define	COMMAND_SAVE	1
#define	COMMAND_NOSAVE	0

struct bootloader_config default_config =
  {
  autoboot	: 0,
  autoactive	: 0,
  console	: NORMAL,
  last_command	: "chain 0"
  };

void
bootloader_init (struct block_io_ops *io)
{
  struct bootloader_config *config;

  // saved region must be in 0xc00
  assert (sizeof bootloader < 0xc00);

  // Check saved configuration.
  if (bootloader_saved.magic != BOOTLOADER_MAGIC)
    {
      printf ("no saved config.\n");
      config = &default_config;
    }
  else
    {
      // copy saved configuration.
      config = &bootloader_saved.config;
    }
  memcpy (&bootloader.config, config, sizeof *config);

  // Reset console.
  bios_console_init (bootloader.config.console);

  // Copy boot code. (real-mode jmp)
  memcpy (bootloader.jmp_code, bootloader_saved.jmp_code, 3);

  // Setup boot configuration area.
  bootloader.magic = BOOTLOADER_MAGIC;

  // Set Disk I/O ops.
  bootloader.work.io = io;

  // Scratch buffer.
  bootloader.work.scratch = (uint8_t *)SCRATCH_AREA;

  // Find Hard disk drives all.
  mbr_init ();

  // Find NetBSD disklabel.
  netbsd_disklabel_init ();

  // Find SysVBFS partition.
  if (sysvbfs_init ())
    shell_command_register (&load_cmd);

  // Install command.
  shell_command_register (&chain_cmd);
  shell_command_register (&active_cmd);
  shell_command_register (&id_cmd);
  shell_command_register (&info_cmd);
  shell_command_register (&autoboot_cmd);
  shell_command_register (&autoactive_cmd);
  shell_command_register (&console_cmd);

  // If autoboot enable, boot here.
  if (config->autoboot && check_autoboot ())
    do_autoboot ();
}

//
// Shell command.
//
uint32_t
info (int32_t argc, const char *argv[])
{
  struct bootloader_mbr *conf;
  struct disklabel *d;
  int i;

  if (argc < 2)
    {
      printf ("info [mbr, disklabel, bfs, config]\n");
      return 1;
    }

  if (strncmp (argv[1], "mbr", 3) == 0)
    {
      // Print all partitions.
      printf ("%d drive exists.\n", bootloader.n_drive);
      mbr_print ();
    }

  if (strncmp (argv[1], "disklabel", 9) == 0)
    {
      for (i = 0; i < bootloader.ndisklabel; i++)
	{
	  conf = bootloader.netbsd[i].mbr;
	  d = &bootloader.netbsd[i].disklabel;
	  printf ("Disk %d, Partition %d\n", conf->drive, conf->index);
	  netbsd_partition_foreach (d, netbsd_partition_print);
	}
    }

  if (strncmp (argv[1], "bfs", 3) == 0)
    {
      for (i = 0; i < bootloader.nbfs; i++)
	{
	  struct bootloader_bfs *bb = bootloader.bfs + i;
	  conf = bb->mbr;
	  printf ("SysVBFS index %d: drive %d, mbr partition %d, offset %d\n",
		  i, conf->drive, conf->index, bb->partition->p_offset);
	  sysvbfs_ls (bb);
	}
    }

  if (strncmp (argv[1], "config", 6) == 0)
    {
      // Bootloader options.
      struct bootloader_config *config = &bootloader.config;
      printf ("console %s\n", config->console == SERIAL ? "serial" : "monitor");
      printf ("autoboot %d\n", config->autoboot);
      printf ("autoactive %d\n", config->autoactive);
      printf ("previous boot command: %s\n", config->last_command);
    }

  return 0;
}

uint32_t
chain (int32_t argc, const char *argv[])
{
  int boot_index;

  if (!__config_index (&boot_index, argc, argv))
    return 1;

  printf ("Chain boot: %d\n", boot_index);

  // Change boot indicator.
  if (bootloader.config.autoactive)
    mbr_partition_activate (boot_index);

  // Save bootloader configuration before boot.
  __config_save (COMMAND_SAVE);

  // Do chain boot.
  return mbr_chain_boot (boot_index);
}

uint32_t
active (int32_t argc, const char *argv[])
{
  int boot_index;

  if (!__config_index (&boot_index, argc, argv))
    return 1;

  mbr_partition_activate (boot_index);
  __config_save (COMMAND_NOSAVE);

  return 0;
}

uint32_t
id (int32_t argc, const char *argv[])
{
  int boot_index;
  int id;

  if (!__config_index (&boot_index, argc, argv))
    return 1;

  if (argc < 3)
    goto usage;
  id = atoi (argv[2]);
  if (id < 0 || id > 255)
    goto usage;

  mbr_partition_id (boot_index, id);
  __config_save (COMMAND_NOSAVE);

  return 0;

 usage:
  printf ("id index#(0-%d) partition_id(0-255)\n",
	  bootloader.n_mbr_partition - 1);
  return 1;
}

uint32_t
load (int32_t argc, const char *argv[])
{
  int partition;

  if (argc < 2)
    {
      printf ("load index filename\n");
      return 1;
    }

  partition = atoi (argv[1]);
  if (partition < 0 || partition >= bootloader.nbfs)
    {
      printf ("invalid index.\n");
      return 1;
    }

  if (argc < 3)
    {
      printf ("load index filename.\n");
      return 1;
    }

  // Save bootloader configuration.
  __config_save (COMMAND_SAVE);
  return sysvbfs_boot (bootloader.bfs + 0, argv[2]);
}

uint32_t
autoboot (int32_t argc, const char *argv[])
{
#define	MAX(a, b)  (((a) > (b)) ? (a) : (b))
  int sec;

  if (argc < 2)
    {
      printf ("autoboot timeout(sec)\nautoboot 0 disable autoboot.\n");
      return 1;
    }

  sec = atoi (argv[1]);

  if (sec > 0)
    bootloader.config.autoboot = MAX (3, sec);
  else
    bootloader.config.autoboot = 0;

  __config_save (COMMAND_NOSAVE);

  return 0;
}

bool
check_autoboot ()
{
  // Check geometory changed from previous boot.
  size_t checklen = sizeof (struct bootloader) -
    sizeof (struct bootloader_work) - sizeof (struct bootloader_config);
  int diff;

  if ((diff = memcmp (&bootloader, &bootloader_saved, checklen)) != 0)
    {
      printf ("Geometory changed (%d/%d). autoboot off.\n", diff, checklen);
      bootloader.config.autoboot = FALSE;
      return FALSE;
    }
  return TRUE;
}

void
do_autoboot ()
{
  bios_func_t kbd_poll_1sec;
  char *cmd = bootloader.config.last_command;

  printf ("autoboot (%s), press any key to stop.\n", cmd);

  int i;
  for (i = bootloader.config.autoboot; i > 0; i--)
    {
      if (call16 (kbd_poll_1sec, 0, 0) != 0)
	goto stop;
      printf ("%d ", i);
    }
  memcpy (shell_command_buffer, cmd, PROMPT_BUFFER_SIZE);
  shell_command_exec (cmd);
 stop:
  printf ("\n");
}

uint32_t
autoactive (int32_t argc, const char *argv[])
{
  if (argc < 2)
    {
      printf ("autoactive on/off.\n");
      return 1;
    }

  bootloader.config.autoactive = strncmp (argv[1], "on", 2) == 0;
  __config_save (COMMAND_NOSAVE);

  return 0;
}

uint32_t
console (int32_t argc, const char *argv[])
{
  enum console_device *cons = &bootloader.config.console;

  if (argc < 2)
    {
      printf ("console monitor/serial\n");
      return 1;
    }

  if (strncmp (argv[1], "serial", 6) == 0)
    *cons = SERIAL;
  else if (strncmp (argv[1], "monitor", 6) == 0)
    *cons = NORMAL;
  else
    {
      printf ("unknown console type.\n");
      return 1;
    }

  bios_console_init (*cons);
  __config_save (COMMAND_NOSAVE);

  return 0;
}

//
// Misc utils.
//
bool
bootloader_drive_change (uint8_t drive)
{

  if (bios_disk_drive (bootloader.work.io->cookie, 0x80 | drive))
    {
      bootloader.work.current_drive = drive;
      return TRUE;
    }

  return FALSE;
}

bool
__config_save (bool save_command)
{
  int i;
  char *p = bootloader.config.last_command;
  // keep last command.
  if (save_command)
    {
      memcpy (p, shell_command_buffer, PROMPT_BUFFER_SIZE);
      for (i = 0; i < PROMPT_BUFFER_SIZE; i++)
	if (p[i] == '\0')
	  p[i] = ' ';
      for (i = PROMPT_BUFFER_SIZE - 1; p[i] == ' ' && i >= 0; i--)
	p[i] = '\0';
    }

  if (!(bootloader_drive_change (0) && bootloader.work.io->write_n
	(bootloader.work.io->cookie, (uint8_t *)&bootloader, 1, 6)))
    {
      printf ("failed to save configuration.\n");
      return FALSE;
    }
  printf ("config saved.\n");

  return TRUE;
}

bool
__config_index (int *idx, int32_t argc, const char *argv[])
{
  assert (bootloader.work.setuped);

  if (argc < 2)
    {
      printf ("Specify configuration index number (0-%d).\n",
	      bootloader.n_mbr_partition - 1);
      return FALSE;
    }

  *idx = atoi (argv[1]);
  if (*idx < 0 || *idx > bootloader.n_mbr_partition)
    {
      printf ("valid index: 0-%d\n", bootloader.n_mbr_partition);
      return FALSE;
    }

  return TRUE;
}
