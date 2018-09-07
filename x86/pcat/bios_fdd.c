
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
#include <assert.h>
#include <sys/console.h>
#include <pcat/bios.h>
#include <string.h>
#ifdef BIOS_BOUNCE_BUFFER
#include "gdt.h"
#endif

#ifdef BIOSIO_DEBUG
#define	DPRINTF(fmt, args...)	printf(fmt, ##args)
#else
#define	DPRINTF(arg...)		((void)0)
#endif

STATIC bool convert_lba (struct bios_int13_packet *, int);

void *
bios_fdd_init (void *self, uint8_t drive)
{
  struct bios_fdd *d = (struct bios_fdd *)self;
  struct bios_int13_packet *packet = &d->packet;

  if (!bios_fdd_drive (self, drive))
    {
      printf ("invalid floppy drive %d\n", drive);
      return NULL;
    }
  d->drive = drive;
  // Access buffer size is 512byte.
  packet->n_sector = 1;

  // BIOS access buffer.
#ifdef BIOS_BOUNCE_BUFFER
  packet->segment = GDT_DATA32_BIOS_BASE >> 4;
  packet->offset = (uint16_t)((addr_t)d->buffer - GDT_DATA32_BIOS_BASE);
  // packet itself
  d->packet_addr = (uint16_t)((addr_t)packet - GDT_DATA32_BIOS_BASE);
#else
  packet->segment = 0;
  packet->offset = (uint16_t)(addr_t)d->buffer;
  d->packet_addr = (uint16_t)(addr_t)packet;
#endif

  return self;
}

bool
bios_fdd_drive (void *self, uint8_t drive)
{
  struct bios_fdd *d = (struct bios_fdd *)self;
  uint16_t ret;

  //  ret = call16 (bios_int13, 0, drive); // Reset command.
  ret = call16 (bios_int13, 0x1500, drive); // Inquire disk type.

  if (ret >> 8 == 0x03)	// Harddisk.
    {
      DPRINTF ("%s: not a floppy disk.\n", __FUNCTION__, ret);
      return FALSE;
    }

  d->packet.drive = drive;
  DPRINTF ("%s: set drive 0x%x state:0x%x\n", __FUNCTION__,  drive,
	   ret >> 8);
  return TRUE;
}

bool
convert_lba (struct bios_int13_packet *packet, int lba)
{
  int track, head, sector;

  track = lba / 36;	// 36 sector/track.
  if (track >= 80)
    {
      DPRINTF ("LBA %d invalid.\n");
      return FALSE;
    }

  sector = lba % 36;
  if ((head = sector > 17 ? 1 : 0))	// 2 head/track
    sector -= 18;
  sector++;	// count from 1 not 0.

  packet->track = track;
  packet->head = head;
  packet->sector = sector;

  DPRINTF ("LBA %d -> track %d, head %d, sector %d\n", lba, track, head, sector);
  return TRUE;
}

bool
bios_fdd_read (void *self, uint8_t *buf, daddr_t lba)
{
  struct bios_fdd *d = (struct bios_fdd *)self;
  struct bios_int13_packet *packet = &d->packet;
  uint16_t error = 0;
  int retry_cnt = 0;

  if (!convert_lba (packet, lba))
    return FALSE;
  DPRINTF ("%s: drive %x sector %d (T%d/H%d/S%d)\n", __FUNCTION__,
	   packet->drive, lba, packet->track, packet->head, packet->sector);
 retry:
  packet->command = INT13_READ;
  error = call16 (bios_int13_rw, d->packet_addr, 0);

  if (error)
    {
      if (((error & 0xff00) == 0x8000) && // timeout
	  retry_cnt++ < BIOS_DISKIO_RETRY_MAX)
	{
	  bios_disk_reset (d->drive);
	  goto retry;
	}
      printf ("%s: 0x%x\n", __FUNCTION__, error);
      return FALSE;
    }
  memcpy (buf, d->buffer, DEV_BSIZE);

  return TRUE;
}

bool
bios_fdd_read_n (void *self, uint8_t *buf, daddr_t lba, int n)
{
  int i;

  for (i = 0; i < n; i++, buf += DEV_BSIZE, lba++)
    {
      if (!bios_fdd_read (self, buf, lba))
	return FALSE;
    }

  return TRUE;
}

bool
bios_fdd_write (void *self, uint8_t *buf, daddr_t lba)
{
  struct bios_fdd *d = (struct bios_fdd *)self;
  struct bios_int13_packet *packet = &d->packet;
  uint16_t error = 0;
  int retry_cnt = 0;

  if (!convert_lba (packet, lba))
    return FALSE;
  packet->command = INT13_WRITE;

 retry:
  memcpy (d->buffer, buf, DEV_BSIZE);
  error = call16 (bios_int13_rw, d->packet_addr, 0);

  DPRINTF ("%s: drive %x sector %d (T%d/H%d/S%d)\n", __FUNCTION__,
	   packet->drive, lba, packet->track, packet->head, packet->sector);
  if (error)
    {
      if (((error & 0xff00) == 0x8000) && // timeout
	  retry_cnt++ < BIOS_DISKIO_RETRY_MAX)
	{
	  bios_disk_reset (d->drive);
	  goto retry;
	}
      printf ("%s: 0x%x\n", __FUNCTION__, error);
      return FALSE;
    }

  return TRUE;
}

bool
bios_fdd_write_n (void *self, uint8_t *buf, daddr_t lba, int n)
{
  int i;

  for (i = 0; i < n; i++, buf += DEV_BSIZE, lba++)
    {
      if (!bios_fdd_write (self, buf, lba))
	return FALSE;
    }

  return TRUE;
}
