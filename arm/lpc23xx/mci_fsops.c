
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

// SD Card filesystem interface.
#include <sys/system.h>
#include <sys/console.h>
#include <mci.h>
#include <sysvbfs/bfs.h>
#include <errno.h>

STATIC struct mci_context mci_context;

int
mmc_init (struct block_io_ops *ops)
{
  struct mci_context *ctx = &mci_context;
  struct mci_controller *controller = &ctx->controller;

  ops->cookie = ctx;
  ops->drive = mmc_drive;
  ops->read = mmc_read;
  ops->read_n = mmc_read_n;
  ops->write = mmc_write;
  ops->write_n = mmc_write_n;

  controller->bus_width = SDCARD_WIDTH_4BIT;
  controller->use_dma = TRUE;
  controller->polling = TRUE;
  controller->slow_clock = FALSE;

  ctx->debug = FALSE;	// debug option causes timing problem.
  ctx->offset = 0;

  return mci_init (ctx) ? 0 : ENXIO;
}

bool
mmc_drive (void *self, uint8_t drive __attribute__((unused)))
{
  struct mci_context *ctx = (struct mci_context *)self;

  // Only one drive.
  mci_card_init (ctx);

  return TRUE;
}

bool
mmc_read_n (void *self, uint8_t *buf, daddr_t lba, int n)
{
  int i;

  for (i = 0; i < n; i++, buf += DEV_BSIZE, lba++)
    {
      if (!mmc_read (self, buf, lba))
	return FALSE;
    }

  return TRUE;
}

bool
mmc_write_n (void *self, uint8_t *buf, daddr_t lba, int n)
{
  int i;

  for (i = 0; i < n; i++, buf += DEV_BSIZE, lba++)
    {
      if (!mmc_write (self, buf, lba))
	return FALSE;
    }

  return TRUE;
}

