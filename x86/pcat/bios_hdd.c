
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
STATIC bool ext_int13_available (void);

void *
bios_disk_init (void *self, uint8_t drive)
{
  struct bios_hdd *d = (struct bios_hdd *)self;
  struct disk_address_packet *p = &d->disk_address_packet;
  uint32_t packet_addr = (uint32_t)p;
  uint32_t command_addr = (uint32_t)&d->drive;

  if (!ext_int13_available ())
    {
      printf ("%s: Too old BIOS.\n", __FUNCTION__);
      return NULL;
    }

#ifdef BIOS_BOUNCE_BUFFER
  d->packet_addr = (uint16_t)(packet_addr - GDT_DATA32_BIOS_BASE);
  d->command_addr = (uint16_t) (command_addr - GDT_DATA32_BIOS_BASE);
  p->buffer32 = ((GDT_DATA32_BIOS_BASE << 12) & 0xffff0000) |
    (((uint32_t)d->buffer - GDT_DATA32_BIOS_BASE) & 0x0000ffff);
#else
  d->packet_addr = (uint16_t)packet_addr;
  d->command_addr = (uint16_t)command_addr;
  p->buffer32 = (uint32_t)d->buffer;
#endif
  p->size_of_packet = 0x10;	// 32bit mode.
  p->nblocks = 1;

  assert (!(d->packet_addr & 0xffff0000));

  if (!bios_disk_drive (self, drive))
    {
      DPRINTF ("invalid hard disk drive 0x%x\n", drive);
      return NULL;
    }

  return self;
}

bool
ext_int13_available ()
{
  uint16_t ret;

  DPRINTF ("Extended Int 13h ");
  if ((ret = call16 (bios_int13_ext_present, 0, 0)) == 0xffff)
    {
      DPRINTF ("not supported.\n");
      return FALSE;
    }
  else
    {
      DPRINTF ("Version %xh. support bit:0x%x\n", ret >> 8, ret & 0xff);
    }
  // bit
  //  0	Device access using the packet structure.
  //  1	Drive locking and ejecting.
  //  2	Enhanced Disk Drive support.
  return (ret & 0x5) == 0x5;
}

bool
bios_disk_drive (void *self, uint8_t drive)
{
  struct bios_hdd *d = (struct bios_hdd *)self;
  uint16_t ret;

  ret = call16 (bios_int13, 0x1500, drive); // Inquire disk type.
  if (ret >> 8 != 0x03)	// Harddisk.
    {
      return FALSE;
    }
  d->drive = drive;

  DPRINTF ("%s: set drive 0x%x\n", __FUNCTION__,  drive);
  return TRUE;
}

void
bios_disk_info (void *self, uint8_t drive)
{
  struct bios_hdd *d = (struct bios_hdd *)self;
  struct disk_parameters *p = (struct disk_parameters *)d->buffer;
  uint16_t parameter_buffer_addr;
  uint32_t edd_conf_addr;
  uint16_t ret;
  uint8_t odrive;
  uint32_t disksize;
  char unit;

  odrive = d->drive;
  d->drive = drive;
  d->command = INT13_EXT_GET_PARAM;
  p->size = sizeof (struct disk_parameters);
#ifdef BIOS_BOUNCE_BUFFER
  parameter_buffer_addr = (uint32_t)p - GDT_DATA32_BIOS_BASE;
#else
  parameter_buffer_addr = (uint32_t)p;
#endif
  ret = call16 (bios_int13_ext, parameter_buffer_addr, d->command_addr);
  ret = ret;

  edd_conf_addr = p->edd_config_ptr;
  edd_conf_addr = ((edd_conf_addr & 0xffff0000) >> 12) |
    (edd_conf_addr & 0xffff);

  if (p->n_sector[0] & 0x200000)
    {	// GB
      unit = 'G';
      disksize = p->n_sector[0] >> 21 | (p->n_sector[1] & 0x1fffff) << 11;
    }
  else	// MB
    {
      unit = 'M';
      disksize = p->n_sector[0] >> 11 | (p->n_sector[1] & 0x7ff) << 21;
    }

  printf ("%d%cB, %d cyl, %d head, %d sect, %d bytes/sect, flags=0x%x, EDD:0x%x\n",
	  disksize, unit, p->n_cylinders, p->n_heads, p->n_sectors_per_track,
	  p->n_bytes_per_sector, p->flags, edd_conf_addr);

  d->drive = odrive;
}

// Ext 13h.
bool
bios_disk_read (void *self, uint8_t *buf, daddr_t lba)
{
  struct bios_hdd *d = (struct bios_hdd *)self;
  uint16_t error;
  int retry_cnt = 0;

 retry:
  d->disk_address_packet.lba[0] = lba;
  d->command = INT13_EXT_READ;
  error = call16 (bios_int13_ext, d->packet_addr, d->command_addr);
  DPRINTF ("%s: drive %x sector %d\n", __FUNCTION__, d->drive, lba);

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
bios_disk_read_n (void *self, uint8_t *buf, daddr_t lba, int n)
{
  int i;

  for (i = 0; i < n; i++, buf += DEV_BSIZE, lba++)
    {
      if (!bios_disk_read (self, buf, lba))
	return FALSE;
    }

  return TRUE;
}

bool
bios_disk_write (void *self, uint8_t *buf, daddr_t lba)
{
  struct bios_hdd *d = (struct bios_hdd *)self;
  uint16_t error;
  int retry_cnt = 0;

retry:
  d->disk_address_packet.lba[0] = lba;
  d->command = INT13_EXT_WRITE;
  memcpy (d->buffer, buf, DEV_BSIZE);

  error = call16 (bios_int13_ext, d->packet_addr, d->command_addr);
  DPRINTF ("%s: drive %x sector %d\n", __FUNCTION__, d->drive, lba);
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
bios_disk_write_n (void *self, uint8_t *buf, daddr_t lba, int n)
{
  int i;

  for (i = 0; i < n; i++, buf += DEV_BSIZE, lba++)
    {
      if (!bios_disk_write (self, buf, lba))
	return FALSE;
    }

  return TRUE;
}
