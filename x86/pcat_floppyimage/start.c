
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
#include <sysvbfs/bfs.h>
#include <sys/delay.h>

#include <pcat/a20.h>
#include <pcat/bios.h>
#include <machine/elf.h>
#include <sys/disklabel_netbsd.h>

#define	BFS_SUPERBLOCK_SIZE	1024
#define	BFS_DIRENT_SIZE		512
#define	SCRATCH_AREA		(16 * 1024 * 1024)
#define	KERNEL_FILENAME		"kernel.elf"
STATIC uint8_t bfs_superblock[BFS_SUPERBLOCK_SIZE];
STATIC uint8_t bfs_dirent[BFS_DIRENT_SIZE];
STATIC uint8_t bios_fdd_context[sizeof (struct bios_fdd) + DEV_BSIZE];

#ifdef DEBUG
#define	DPRINTF(fmt, args...)	printf(fmt, ##args)
#else
#define	DPRINTF(arg...)		((void)0)
#endif

__BEGIN_DECLS
void machine_startup (int boot_drive) __attribute__ ((noreturn, regparm (1)));
__END_DECLS

STATIC bool load_kernel (int);
STATIC bool bfs_load_kernel (uint32_t, int);

STATIC struct block_io_ops bios_io =
  {
    NULL,
    bios_fdd_drive,
    bios_fdd_read,
    bios_fdd_read_n,
    bios_fdd_write,
    bios_fdd_write_n
  };

static bool a20_ok __attribute__((section (".data")));
void __attribute__ ((noreturn, regparm (1)))
machine_startup (int boot_drive)
{
  // Symbols provided from ldscript.
#ifdef DEBUG
  extern char _stack_start[];
  extern char data_start[], data_end[];
#endif
  extern char bss_start[], bss_end[];
  char *p;

  delay_init ();	// A20 gate needs delay.
  // Gate A20 on
  a20_ok = a20_enable (TRUE);

  // Clear BSS
  for (p = bss_start; p < bss_end; p++)
    *p = 0;

  // Initialize BIOS console.
#ifdef SERIAL_CONSOLE
  bios_console_init (SERIAL);
#else
  bios_console_init (NORMAL);
#endif
  // Now OK to use printf.
  DPRINTF ("stack: 0x%A-0x%A %dbyte\n", _stack_start, bss_end,
	  _stack_start - bss_end);
  DPRINTF ("data: 0x%A-0x%A %dbyte\n", data_start, data_end,
	  data_end - data_start);
  DPRINTF ("bss: 0x%A-0x%A %dbyte\n", bss_start, bss_end,
	  bss_end - bss_start);
  DPRINTF ("A20: %sabled.\n", a20_ok ? "en" : "dis");
  DPRINTF ("boot drive: 0x%x\n", boot_drive);

  //  delay_calibrate ();	//test

  // Setup Disk I/O ops.
  bios_io.cookie = bios_fdd_init (bios_fdd_context, boot_drive);

  // Load kernel and jump.
  load_kernel (boot_drive);
  /* NOTREACHED */

  printf ("boot failed.\n");
  while (/*CONSTCOND*/1)
    cpu_sleep ();
}

bool
load_kernel (int arg)
{
  uint32_t start_sector;

  if (sysvbfs_partition_find ((struct disklabel *)0x7e00, &start_sector))
    {
      return bfs_load_kernel (start_sector, arg);
    }

  return FALSE;
}

bool
bfs_load_kernel (uint32_t start_sector, int arg)
{
  struct bfs_init_arg init_arg;
  struct bfs bfs;
  struct bfs *b;
  uint8_t *scratch = (uint8_t *)SCRATCH_AREA;
  size_t readed;
  uint32_t entry;
  int err;

  init_arg.start_sector = start_sector;
  init_arg.superblock_size = BFS_SUPERBLOCK_SIZE;
  init_arg.superblock = bfs_superblock;
  init_arg.dirent_size = BFS_DIRENT_SIZE;
  init_arg.dirent = bfs_dirent;
  init_arg.io_ops = &bios_io;
  init_arg.debug = FALSE;
  init_arg.bfs = &bfs;

  DPRINTF ("bfs sector:%d\n", start_sector);

  if ((b = bfs_init (&init_arg)) == NULL)
    return FALSE;

  if ((err = bfs_file_read (b, KERNEL_FILENAME, scratch,
			    0xffffffff, &readed)) != 0)
    {
      printf ("%s: read failed. (%d)\n", KERNEL_FILENAME, err);
      return FALSE;
    }

  if (!load_elf (scratch, &entry))
    return FALSE;

  //  __asm volatile ("jmp *%0":: "r"(entry));
  __asm volatile ("movl %1, %%eax;jmp *%0":: "c"(entry), "g"(arg));
  // NOTREACHED

  return FALSE;
}

