
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
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/ringbuffer.h>

#include <reg.h>
#include <bfs_simple.h>
#include <sysvbfs/bfs_datafile.h>
#include <string.h>

#include "local.h"
#include "data.h"
#include <sys/mmc.h>
#include <mci_dma.h>

STATIC struct thread_control storage_tc __attribute__((aligned (4)));
STATIC void storage_thread (uint32_t);
STATIC continuation_func storage_main __attribute__((noreturn));

STATIC struct bfs *filesystem;
STATIC struct bfs_datafile logfile;
STATIC struct mci_dma dma;

STATIC volatile int storage_thread_ready;

thread_t
storage_init ()
{
  thread_t th;

  th = thread_create_no_stack (&storage_tc, "storage", storage_thread, 0);
  thread_start (th);
  // Wait storage initialize.
  while (!storage_thread_ready)
    thread_block (NULL);

  return th;
}

void
storage_thread (uint32_t arg __attribute__((unused)))
{
  static struct block_io_ops sdcard;

  // Setup DMA buffer. Steal USB device memory.
  mci_dma_alloc (&dma);
  // storage:PRI_HIGH, data:PRI_APPHI, controller: PRI_LOW
  thread_priority (current_thread, PRI_HIGH);
  mmc_init (&sdcard);

  intr_enable ();
  filesystem = bfs_simple_init (&sdcard, 0, dma.read_buffer,
				dma.fs_control_block);
  assert (filesystem);
  storage_thread_ready = 1;
  // Setuped. wakeup master.
  thread_wakeup (shell_th);
  // Block and continuation to next.
  thread_block (storage_main);
  // NOTREACHED
}

void
storage_main ()
{
  int err;

  intr_enable ();

  switch (log_status_flag)
    {
    case LOG_STATUS_IDLE:
      DPRINTF ("idling. no write.\n");
      break;

    case LOG_STATUS_SAMPLING:
      // This thread is the highest priority thread of application.
      // So never be preempted by the other threads.
      DPRINTF ("write start\n");
      if ((err = bfs_datafile_write
	   (&logfile, dma.write_buffer[dma.write_buffer_bank ^ 1])) != 0)
	{
	  DPRINTF ("can't write log file. %d\n", err);
	}
      // Confirm current filesize.
      //(This is not needed. but for unexpected hang up.)
      if ((err = bfs_datafile_metadata_update (&logfile)) != 0)
	{
	  DPRINTF ("can't update metadata. %d\n", err);
	}
      DPRINTF ("write end\n");
      break;

    case LOG_STATUS_STOP:
      if ((err = bfs_datafile_close (&logfile)) != 0)
	{
	  DPRINTF ("can't close log file. %d\n", err);
	}
      log_status_flag = LOG_STATUS_IDLE;
      thread_wakeup_once (data_th);
      DPRINTF ("file closed\n");
      break;
    }

  thread_block (storage_main);
  // NOTREACHED
  while (/*CONSTCOND*/1)
    ;
}

void
storage_buffer_flip ()
{

  dma.write_buffer_bank ^= 1;
}

uint32_t *
storage_current_buffer ()
{

  return (uint32_t *)dma.write_buffer[dma.write_buffer_bank];
}

bool
storage_file_open (const char *name)
{
  struct bfs_fileattr attr;

  memset (&attr, 0xff, sizeof attr);	/* Set VNOVAL all */

  if (bfs_datafile_open (filesystem, name, &attr, &logfile) == 0)
    return TRUE;

  return FALSE;
}

void
storage_ls ()
{

  bfs_dump (filesystem);
}

int
storage_nfile ()
{

  return filesystem ? filesystem->n_inode - 1 /*. and ..*/: 0;
}


void
storage_rm (const char *filename)
{

  bfs_file_delete (filesystem, filename);
}
