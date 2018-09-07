
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

// Flash ROM write for LPC23XX
#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "lpc23xx.h"

#define	FREQ_KHZ_DEFAULT	12000
#define	BPS_DEFAULT		B115200
#define	CHUNK_SIZE		4096	// MAX value. don't increase.

static int load_image (int, FILE *, size_t, long int);
static int load_image_to_ram (int, FILE *, uint32_t, size_t);
static int flash_erace (int, size_t);
static int flash_write (int, FILE *);
static int copy_ram_to_flash (int, uint32_t, size_t, int, int);
static int open_serial (const char *, speed_t);
static int lpc23xx_flash_offset (long int, int *, int *);
static FILE *open_tmpfile (void);
static void close_tmpfile (FILE *);
static FILE *open_loadfile (const char *, size_t *);
static void usage (void);
static speed_t atobps (const char *);

int DumpFlag;

int
main (int argc, char *argv[])
{
  char device[FILENAME_MAX];
  const char *binary_file;
  size_t binary_size;
  FILE *loadfile;
  int fd = 0;
  int error = 1;
  int i;
  int freq;
  speed_t speed;

  memset (device, 0, sizeof device);
  freq = FREQ_KHZ_DEFAULT;
  speed = BPS_DEFAULT;

  while ((i = getopt(argc, argv, "d:i:f:s:")) != -1)
    {
      switch (i)
	{
	case 'd':	// Device file.
	  if (!optarg)
	    usage ();
	  strncpy (device, optarg, sizeof device);
	  break;
	case 'f':	// ISP Frequency
	  if (!optarg)
	    usage ();
	  freq = atoi (optarg);
	  break;
	case 's':	// Serial line speed.
	  if (!optarg)
	    usage ();
	  speed = atobps (optarg);
	  break;
	}
    }
  argc -= optind;
  argv += optind;

  if (argc < 1)
    {
      fprintf (stderr, "Specify binary file.\n");
      usage ();
    }
  binary_file = argv[0];

  if (!*device)
    {
      // Dump UU-encoded text to stdout.
      DumpFlag = 1;
    }

  fprintf (stderr, "Output Device [ %s %s]\nLPC23xx ISP Frequency [%dkHz]\n",
	   DumpFlag ? "stdout" : device,
	   DumpFlag ? "" : optarg,
	   freq);

  // Open target file.
  if ((loadfile = open_loadfile (binary_file, &binary_size)) == NULL)
    goto error;

  // Write checksum to ARM interrupt vector location 0x14
  if (lpc23xx_vector_fix (loadfile) != 0)
    goto error;

  // Connect ISP.
  if ((fd = open_serial (device, speed)) == 0)
    goto error;

  // Initialize LP23xx's ISP.
  if (lpc23xx_isp_init (fd, freq) != 0)
    goto error;

  // Erace sectors to be loaded.
  if (flash_erace (fd, binary_size) != 0)
    goto error;

  // Load binary to Flush ROM.
  if (flash_write (fd, loadfile) != 0)
    goto error;

  // Echo on.
  error = lpc23xx_isp_echo (fd, 1);

 error:
  fprintf (stderr, "%s\n", error ? "Failed." : "Success.");

  if (loadfile)
    fclose (loadfile);
  if (fd)
    close (fd);

  return error;
}

int
open_serial (const char *device, speed_t speed)
{
  struct termios t;
  int desc;

  if (DumpFlag)
    {
      // Output to stdout.
      return fcntl (STDOUT_FILENO, F_DUPFD, 0);
    }

  if ((desc = open (device, O_RDWR)) == 0)
    {
      perror (device);
      return 0;
    }

  tcsetpgrp(desc, getpgrp ());
  if (tcgetattr (desc, &t) != 0)
    {
      perror (device);
      goto error;
    }

  // 8N1
  cfsetispeed(&t, speed);
  cfsetospeed(&t, speed);
  t.c_cflag &= ~(CSIZE|PARENB);
  t.c_cflag |= CS8;
  t.c_iflag &= ~(ISTRIP|ICRNL);
  t.c_oflag &= ~OPOST;
  t.c_lflag &= ~(ICANON|ISIG|IEXTEN|ECHO);
  t.c_cc[VMIN] = 1;
  t.c_cc[VTIME] = 0;

  if (tcsetattr (desc, TCSANOW, &t) != 0)
    {
      perror ("tcsetattr (1)");
      goto error;
    }

  // Drain buffer.
  if (tcflush (desc, TCIOFLUSH) == -1)
    {
      perror ("tcflush");
      goto error;
    }

  if (tcdrain (desc) != 0)
    {
      perror ("tcdrain");
      goto error;
    }

  return desc;

 error:
  close (desc);

  return 0;
}

int
flash_write (int fd, FILE *loadfile)
{
  FILE *uufile;
  long int offset;
  int error = 1;

  for (offset = 0, uufile = NULL;; offset += CHUNK_SIZE)
    {
      size_t chunk_size;
      // Prepare UU-encoded file.
      if ((uufile = open_tmpfile ()) == NULL)
	return 1;
      chunk_size = lpc23xx_uuencode (loadfile, offset, CHUNK_SIZE, uufile);
      if (chunk_size == 0)
	break;

      // Load UU-encoded image to LPC23xx Flash-ROM.
      if ((error = load_image (fd, uufile, chunk_size, offset)) != 0)
	break;

      close_tmpfile (uufile);
      uufile = NULL;
    }
  if (uufile)
    close_tmpfile (uufile);

  return error;
}

int
load_image (int fd, FILE *uu, size_t sz, long int flash_offset)
{
  int sector, offset;

  rewind (uu);

  // Load image to RAM.
  if (load_image_to_ram (fd, uu, 0x40000200, sz) != 0)
    return 1;

  // Determine target Flash sector.
  if (lpc23xx_flash_offset (flash_offset, &sector, &offset) != 0)
    return 1;
  fprintf (stderr, "0x%x: sector 0x%x offset 0x%x\n",
	   (int32_t)flash_offset, sector, offset);

  // Copy RAM-image to Flash.
  if (copy_ram_to_flash (fd, 0x40000200, sz, sector, flash_offset) != 0)
    return 1;

  return 0;
}

int
load_ram_checksum_responce (int fd)
{
  if (DumpFlag)
    return 0;

  char buf[64];
  ssize_t sz;

  if ((sz = read (fd, buf, sizeof buf)) == -1)
    {
      perror (__FUNCTION__);
      return 1;
    }

  if (strncmp (buf, "OK\r\n", 4) != 0)
    {
      buf[sz] = '\0';
      fprintf (stderr, "Bad checksum responce. %s.\n", buf);
      return 1;
    }

  return 0;
}

int
load_image_to_ram (int fd, FILE *uu, uint32_t ram_addr, size_t sz)
{
  char buf[64];
  int i;

  sprintf (buf, "W %d %zd", ram_addr, sz);
  if (lpc23xx_isp_command (fd, buf) != 0)
    return 1;

  if (DumpFlag)
    {
      sprintf (buf, "%zd\n", sz);
      write (fd, buf, strlen (buf));
    }

  while (/*CONSTCOND*/1)
    {
      for (i = 0; i < 21; i++)
	{
	  if (fgets (buf, sizeof buf, uu) == NULL)
	    goto done;

	  if (write (fd, buf, strlen (buf)) == -1)
	    {
	      perror (__FUNCTION__);
	      return 1;
	    }
	}
      // Checksum per 20 lines.
      if (load_ram_checksum_responce (fd) != 0)
	return 1;
    }
 done:
  if (i && load_ram_checksum_responce (fd) != 0)
    return 1;

  return 0;
}

int
flash_erace (int fd, size_t size)
{
  int sector, off;
  char buf[64];

  if (lpc23xx_flash_offset (size, &sector, &off) != 0)
    return 1;

  sprintf (buf, "P %d %d", 0, sector);
  if (lpc23xx_isp_command (fd, buf) != 0)
    return 1;

  sprintf (buf, "E %d %d", 0, sector);
  if (lpc23xx_isp_command (fd, buf) != 0)
    return 1;

  return 0;
}

int
copy_ram_to_flash (int fd, uint32_t ram_addr, size_t sz, int sector, int offset)
{
  char buf[64];
  size_t size;

  if (offset & 0xff)
    {
      fprintf (stderr, "Flash offset alignemnt error: 0x%x\n", offset);
      return 1;
    }

  if (sz > 4096)
    {
      fprintf (stderr, "copy size too big. %zdbyte > 4096", sz);
      return 1;
    }
  else if (sz > 1024)
    size = 4096;
  else if (sz > 512)
    size = 1024;
  else if (sz > 256)
    size = 512;
  else
    size = 256;

  sprintf (buf, "P %d %d", sector, sector);
  if (lpc23xx_isp_command (fd, buf) != 0)
    return 1;

  sprintf (buf, "C %d %d %zd", offset, ram_addr, size);
  if (lpc23xx_isp_command (fd, buf) != 0)
    return 1;

  return 0;
}

int
lpc23xx_flash_offset (long int total_offset, int *sector, int *offset)
{
  if (total_offset & 0xff)
    {
    }

  if (total_offset < 0x8000)
    {
      *sector = total_offset >> 12;
      *offset = total_offset & 0xfff;
    }
  else if (total_offset < 0x78000)
    {
      total_offset -= 0x8000;
      *sector = (total_offset >> 15) + 8;
      *offset = total_offset & 0x7fff;
    }
  else if (total_offset < 0x7e000)
    {
      total_offset -= 0x78000;
      *sector = (total_offset >> 12) + 22;
      *offset = total_offset & 0xfff;
    }
  else
    {
      fprintf (stderr, "flash over flow (%x)\n", (int32_t)total_offset);
      return 1;
    }

  return 0;
}

char sfn[15];

FILE *
open_tmpfile ()
{
  FILE *sfp;
  int fd = -1;

  memset (sfn, 0, sizeof sfn);
  strncpy (sfn, "./uu.XXXXXX", sizeof sfn - 1);
  if ((fd = mkstemp (sfn)) == -1 || (sfp = fdopen (fd, "w+")) == NULL)
    {
      if (fd != -1)
	{
	  unlink (sfn);
	  close (fd);
	}
      fprintf (stderr, "%s: %s\n", sfn, strerror (errno));
      return NULL;
    }

  return sfp;
}

void
close_tmpfile (FILE *sfp)
{

  fclose (sfp);
  unlink (sfn);
}


FILE *
open_loadfile (const char *filename, size_t *sz)
{
  struct stat sb;
  FILE *f = NULL;

  fprintf (stderr, "Send file [%s]", filename);
  if (stat (filename, &sb) == -1)
    {
      perror ("stat");
      return NULL;
    }
  *sz = sb.st_size;

  if ((f = fopen (filename, "r+")) == NULL)
    {
      fprintf (stderr, "Couldn't open input file %s\n", filename);
      return NULL;
    }
  fprintf (stderr, ": %zd byte\n", *sz);

  return f;
}

void
usage ()
{

  fprintf (stderr, "lpc23xxwrite [-f frequency] [-d device] [-s line speed] "
	   "file\n");
  fprintf (stderr, "EXAMPLE: lpc23xxwrite -f 12000 -d /dev/dtyU0 -s 115200 "
	   "foo.bin\n");
  fprintf (stderr, "ISP Frequency 12MHz, line is /dev/dtyU0, "
	   "speed is 115200bps)\n");
  fprintf (stderr, "When output device is not specified, "
	   "Dump uuencoded text to stdout.\n");
  fprintf (stderr, "[lpc23xxwrite foo.bin | lpc23xxuud > bar.bin]"
	   " generates upload image.\n");
  fprintf (stderr, "Supported line speed: 1200, 9600, 19200, 38400, 57600,"
	   "115200, 230400\n");
  exit (1);
}

speed_t
atobps (const char *a)
{
  int speed = atoi (a);

  switch (speed)
    {
    default:
      break;
    case 1200:
      return B1200;
    case 9600:
      return B9600;
    case 19200:
      return B19200;
    case 38400:
      return B38400;
    case 57600:
      return B57600;
    case 115200:
      return B115200;
    case 230400:
      return B230400;
    }
  usage ();
  return 0;
}
