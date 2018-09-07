
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
#include <string.h>
#include <sys/timer.h>
#include <sys/mmc.h>
#include <sysvbfs/bfs.h>

//#define	MMC_DEBUG
#define	MMC_VERBOSE

#ifdef MMC_DEBUG
#define	DPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif
#ifdef MMC_VERBOSE
#define	LPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	LPRINTF(fmt, args...)	((void)0)
#endif

STATIC void __mmc_wait_busy (void);
STATIC uint8_t __mmc_response (void);
STATIC uint8_t __mmc_command_subr (const uint8_t *);
STATIC bool __mmc_command (const uint8_t *);
STATIC bool __mmc_wait_data (void);
STATIC uint16_t __mmc_data_crc (void);
STATIC bool __mmc_data_command (int, uint32_t, uint8_t *, size_t);
STATIC bool __mmc_nodata_command (int, uint32_t, uint8_t *, size_t);
STATIC void __mmc_command_response (uint8_t);
STATIC bool __mmc_data_write_command (int, uint32_t, uint8_t *, size_t);
STATIC void __mmc_error_response (uint8_t);
STATIC bool __mmc_data_response (uint8_t);
STATIC const uint8_t *mmc_set_command (int, uint32_t);
void __mmc_timer_start (void);
void __mmc_timer_count (void *);
void __mmc_timer_stop (void);

STATIC int __mmc_timer;
STATIC bool __mmc_timer_enable;
#define	MMC_TIMEOUT	100	// 1sec.

struct mmc_command
{
  uint8_t command;
  uint8_t arg[4];
  uint8_t crc;
} __attribute__ ((packed));
#define	MMC_CMD(x)	(x | 0x40)
#define	MMC_CRC(x)	(x | 0x01)	// Stop bit.

int
mmc_init (struct block_io_ops *io)
{

  md_mmc_init ();
  io->drive = mmc_drive;
  io->read = mmc_read;
  io->read_n = mmc_read_n;
  io->write = mmc_write;
  io->write_n = mmc_write_n;

  return 0;
}

bool
mmc_drive (void *self __attribute__((unused)),
	   uint8_t drive __attribute__((unused)))
{

  // Only one drive.
  mmc_cmd_reset ();

  return TRUE;
}

bool
mmc_read (void *self, uint8_t *buf, daddr_t lba)
{
  self = self;
  DPRINTF ("sector %D\n", lba);
  return __mmc_data_command (17, lba * DEV_BSIZE, buf, 512);
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
mmc_write (void *self, uint8_t *buf, daddr_t lba)
{
  self = self;
  DPRINTF ("sector %D\n", lba);
  return __mmc_data_write_command (24, lba * DEV_BSIZE, buf, 512);
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

//----------------------------------------------------------------------
// MMC command.
//----------------------------------------------------------------------
bool
mmc_cmd_reset ()
{
  struct mmc_command cmd0;
  int i;
  uint8_t r;

  //  __mmc_timer_start ();
  do
    {
      memset (&cmd0, 0, sizeof cmd0);
      cmd0.command = MMC_CMD (0);
      cmd0.crc = MMC_CRC (0x94);

      // Reset card. (CMD0)
      md_mmc_deactivate ();
      for (i = 0; i < 10; i++)	// 80 dummy clock
	md_mmc_putc (0xff);		// must be 0xff

      md_mmc_activate ();
      r = __mmc_command_subr ((const uint8_t *)&cmd0);
      md_mmc_deactivate ();
      DPRINTF ("response = %x\n", (uint32_t)r);
    }
  while ((r != 0x01) && __mmc_timer < MMC_TIMEOUT);	// Idle state.
  //  __mmc_timer_stop ();

  while (!mmc_cmd_init ())
    ;

  return TRUE;
}

bool
mmc_cmd_init ()
{

  return __mmc_nodata_command (1, 0, NULL, 0);
}

void
mmc_cmd_ocr ()
{
  uint8_t buf[4];

  __mmc_nodata_command (58, 0, buf, sizeof buf);
}

void
mmc_cmd_csd ()
{
  uint8_t buf[16];

  __mmc_data_command (9, 0, buf, sizeof buf);
}

void
mmc_cmd_cid ()
{
  uint8_t buf[16];

  __mmc_data_command (10, 0, buf, sizeof buf);
}

void
__mmc_wait_busy ()
{
  uint8_t r;

  // XXX timeout.
  while ((r = md_mmc_getc ()) != 0xff)
    DPRINTF ("drain = %x\n", (uint32_t)r);
}

uint8_t
__mmc_response ()
{
  uint8_t r;

  // XXX timeout.
  while ((r = md_mmc_getc ()) == 0xff)
    ;

  return r;
}

uint8_t
__mmc_command_subr (const uint8_t *buf)
{
  int i;

  __mmc_wait_busy ();
  for (i = 0; i < 6; i++)
    md_mmc_putc (buf[i]);

  return  __mmc_response ();
}

bool
__mmc_command (const uint8_t *buf)
{
  uint8_t r;

  // XXX timeout.
  do
    {
      r = __mmc_command_subr (buf);
      DPRINTF ("response = ");
      __mmc_command_response (r);
    }
  while (r != 0);

  return TRUE;
}

bool
__mmc_wait_data ()
{
  uint8_t r;

  // XXX timeout.
  // Wait until Data Token Start Byte.(0xfe)
  while ((r = md_mmc_getc ()) != 0xfe)
    DPRINTF ("r=%x\n", (uint32_t)r & 0xff);
  DPRINTF ("datastart\n");
  return TRUE;
}

uint16_t
__mmc_data_crc ()
{
  uint16_t crc;

  crc = md_mmc_getc () << 8;
  crc |= md_mmc_getc () & 0xff;
  DPRINTF ("CRC=%x\n", (uint32_t)crc & 0xffff);

  return crc;
}

bool
__mmc_data_command (int command, uint32_t arg, uint8_t *buf, size_t n)
{
  size_t i;

  md_mmc_activate ();
  if (!__mmc_command (mmc_set_command (command, arg)))
    return FALSE;
  if (!__mmc_wait_data ())
    return FALSE;

  for (i = 0; i < n; i++)
    {
      buf[i] = md_mmc_getc ();
      //      iprintf ("%x ", (uint32_t)buf[i]);
    }
  __mmc_data_crc ();
  md_mmc_deactivate ();

  return TRUE;
}

bool
__mmc_data_write_command (int command, uint32_t arg, uint8_t *buf, size_t n)
{
  size_t i;
  uint8_t r;
  bool done = FALSE;

  md_mmc_activate ();
  if (!__mmc_command (mmc_set_command (command, arg)))
    return FALSE;
  md_mmc_putc (0xff);	// At least 1byte wait.
  DPRINTF ("send start\n");
  // Data packet.
  md_mmc_putc (0xfe);	//Data token start byte.
  for (i = 0; i < n; i++)
    {
      md_mmc_putc (buf[i]);
    }
  md_mmc_putc (0x00);	// Dummy CRC
  md_mmc_putc (0x00);	// Dummy CRC
  DPRINTF ("send done\n");

  r = __mmc_response ();
  if ((r & 0xe0) == 0)
    __mmc_error_response (r);
  else
    done = __mmc_data_response (r);

  //  iprintf ("%x\n", (uint32_t)r);
  __mmc_wait_busy ();

  md_mmc_deactivate ();

  return done;
}

bool
__mmc_nodata_command (int command, uint32_t arg, uint8_t *buf, size_t n)
{
  size_t i;

  md_mmc_activate ();
  if (!__mmc_command (mmc_set_command (command, arg)))
    return FALSE;
  for (i = 0; i < n; i++)
    {
      buf[i] = md_mmc_getc ();
      DPRINTF ("%x ", (uint32_t)buf[i]);
    }

  md_mmc_deactivate ();

  return TRUE;
}

const uint8_t *
mmc_set_command (int command, uint32_t arg)
{
  static struct mmc_command cmd;
  uint32_t shift;
  int i;

  cmd.command = MMC_CMD (command);
  cmd.crc = MMC_CRC (0);
#if 1
  for (i = 0, shift = 24; i < 4; i++, shift -= 8)
    {
      cmd.arg[i] = (arg >> shift) & 0xff;
    }
#else
  for (i = 0, shift = 0; i < 4; i++, shift += 8)
    {
      cmd.arg[i] = (arg >> shift) & 0xff;
    }
#endif

#ifdef MMC_DEBUG
  uint8_t *r = (uint8_t *)&cmd;
  for (i = 0; i < 6; i++)
    iprintf ("%x ", (uint32_t)r[i]);
  iprintf ("\n");
#endif
  return (uint8_t *)&cmd;
}

void
__mmc_command_response (uint8_t r1)
{
  int i;
  const char *error_message [] =
    {
      "In Idle State",
      "Erase Reset",
      "Illegal Command",
      "Communication CRC error",
      "Erase Sequence error",
      "Address error",
      "Parameter error",
    };

  if (r1 == 0)
    {
      DPRINTF ("Success.\n");
    }
  else
    {
      for (i = 0; i < 7; i++)
	if (r1 & (1 << i))
	  LPRINTF ("%s ", error_message[i]);
      LPRINTF ("\n");
    }
}

bool
__mmc_data_response (uint8_t r)
{

  switch (r & 0x1f)
    {
    case 5:
      DPRINTF ("Success.\n");
      return TRUE;
    case 11:
      LPRINTF ("CRC error.\n");
      break;
    case 13:
      LPRINTF ("Write error.\n");
      break;
    default:
      LPRINTF ("Unknown error. %x\n", (uint32_t)r);
      break;
    }

  return FALSE;
}

void
__mmc_error_response (uint8_t r)
{
  int i;
  const char *error_message [] =
    {
      "Undefined error",
      "Controller error",
      "ECC failed.",
      "Out of range",
      "Card is locked.",
    };

  for (i = 0; i < 5; i++)
    if (r & (1 << i))
      LPRINTF ("%s ", error_message[i]);
  DPRINTF ("\n");
}

void
__mmc_timer_count (void *arg __attribute__((unused)))
{
  __mmc_timer++;
  // reinstall myself.
  if (__mmc_timer_enable)
    while (!timer_schedule_func (__mmc_timer_count, NULL, 10000))  // 10msec.
      ;
}

void
__mmc_timer_start ()
{
  while (!timer_schedule_func (__mmc_timer_count, NULL, 10000))  // 10msec.
    ;
  __mmc_timer = 0;
  __mmc_timer_enable = TRUE;
}

void
__mmc_timer_stop ()
{

  __mmc_timer_enable = FALSE;
}
