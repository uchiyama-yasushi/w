
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
#include <stdlib.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <sys/board.h>
#include <cpu.h>
#include <reg.h>

#include <string.h>
#include <sys/thread.h>
#include <sys/delay.h>
#include <sys/timer.h>

#include <bfs_simple.h>
#include <pcat_mbr.h>
#include <sys/disklabel_netbsd.h>
#include <sys/mmc.h>

#include <mci_dma.h>
#include <filesystem/v7fs/v7fs.h>
#include <filesystem/v7fs/v7fs_inode.h>
#include <filesystem/v7fs/v7fs_datablock.h>
#include <filesystem/v7fs/v7fs_impl.h>

SHELL_COMMAND_DECL (mount);
SHELL_COMMAND_DECL (ls);
SHELL_COMMAND_DECL (cat);

STATIC struct mci_dma dma;
static struct v7fs_self v7fs;

void
board_main (uint32_t arg)
{
  printf ("%s: arg=%x\n", __FUNCTION__, arg);
  shell_command_register (&mount_cmd);
  shell_command_register (&ls_cmd);
  shell_command_register (&cat_cmd);

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

void
board_reset (uint32_t cause __attribute__((unused)))
{

  // Stop timer.
  timer_fini ();

  cpu_reset ();
  while (/*CONSTCOND*/1)
    ;
  // NOTREACHED
}

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | DELAY_CALIBRATE | RAM_CHECK | BUFFERED_CONSOLE_ENABLE;
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{
  // on-board LED
  // GPIO power: always enabled.
  mcu_fast_gpio (TRUE);
  gpio_dir (1, 18, GPIO_OUTPUT); // P1.18 connected to on-board LED.
  *FIO1MASK = 0;
  //LED on
  GPIO_PIN_CLR (1, 18);

  timer_init ();
  dma_init ();
  //  rtc_init ();
  vic_init ();

  //  dac_init ();
}

STATIC struct mci_dma dma;

uint32_t
mount (int32_t argc __attribute__((unused)),
       const char *argv[] __attribute__((unused)))
{
  // Attach filesystem
  struct v7fs_self *_;

  v7fs_core_init(&_, NULL, LITTLE_ENDIAN);

  return 0;
}

int
v7fs_os_init (struct v7fs_self **_,
	      union v7fs_mount_device *mount_device __attribute__((unused)),
	      size_t block_size __attribute__((unused)))
{
  // Initialize MCI module.
  int error;

  if ((error = mmc_init (&v7fs.io)))
    {
      printf ("SD card initialize failed. %d\n", error);
      return error;
    }
  mci_dma_alloc (&dma);

  // Initialize SD-card.
  v7fs.io.drive (v7fs.io.cookie, 0);
  //XXX V7FS offset
  struct mci_context *ctx = (struct mci_context *)v7fs.io.cookie;
  ctx->offset = 33329;
  *_ = &v7fs;

  return 0;
}

void
v7fs_os_fini (struct v7fs_self *_ __attribute__((unused)))
{

}

int readdir_subr (struct v7fs_self *, void *, v7fs_daddr_t, size_t);
struct v7fs_readdir_arg {
  int start;
  int end;
  int cnt;
};

uint32_t
ls (int32_t argc, const char *argv[])
{
  struct v7fs_inode inode;
  struct v7fs_readdir_arg arg;
  v7fs_ino_t ino;

  if (argc < 2)
    return 0;

  ino = atoi (argv[1]);

  v7fs_inode_lookup (&v7fs, &inode, ino);
  arg.start = 0;
  arg.end = v7fs_inode_filesize (&inode) / sizeof (struct v7fs_dirent);
  arg.cnt = 0;
  v7fs_datablock_foreach (&v7fs, &inode, readdir_subr, &arg);

  return 0;
}

int
readdir_subr (struct v7fs_self *_, void *ctx, v7fs_daddr_t blk, size_t sz)
{
  struct v7fs_readdir_arg *p = (struct v7fs_readdir_arg *)ctx;
  void *buf;
  struct v7fs_dirent *dir;
  struct v7fs_inode inode;
  int i, n;

  if (!(buf = scratch_read (_, blk)))
    return EIO;
  dir = (struct v7fs_dirent *)buf;

  n =sz / sizeof (struct v7fs_dirent);

  for (i = 0; (i < n) && (p->cnt < p->end); i++, dir++, p->cnt++)
    {
      if (p->cnt < p->start)
	continue;

      v7fs_inode_lookup (_, &inode, dir->inode_number);

      printf ("inode=%d name=%s %s\n", dir->inode_number, dir->name,
	      v7fs_inode_isdir (&inode) ? "DIR" : "FILE");
      //      v7fs_inode_dump (_, &inode);
    }
  scratch_free (_, buf);

  return 0;
}

int filedump_subr (struct v7fs_self *, void *, v7fs_daddr_t, size_t);

uint32_t
cat (int32_t argc, const char *argv[])
{
  struct v7fs_inode inode;
  v7fs_ino_t ino;

  if (argc < 2)
    return 0;
  ino = atoi (argv[1]);

  if (v7fs_inode_lookup (&v7fs, &inode, ino))
    return 0;
  v7fs_datablock_foreach (&v7fs, &inode, filedump_subr, 0);

  return 0;
}

int
filedump_subr (struct v7fs_self *_, void *ctx, v7fs_daddr_t blk, size_t sz)
{
  ctx=ctx,sz=sz;_=_;
  void *buf;

  if (!(buf = scratch_read (_, blk)))
      return EIO;
  printf ("%s", (char *)buf);

  scratch_free (_, buf);

  return 0;
}

