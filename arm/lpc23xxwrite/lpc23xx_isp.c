
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

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "lpc23xx.h"

static int lpc23xx_isp_autobaud_sync (int);
static int lpc23xx_isp_frequency_set (int, int);

int
lpc23xx_isp_init (int fd, int khz)
{
  if (DumpFlag)
    return 0;

  if (lpc23xx_isp_autobaud_sync (fd) != 0)
    return 1;
  if (lpc23xx_isp_frequency_set (fd, khz) != 0)
    return 1;
  if (lpc23xx_isp_echo (fd, 0) != 0)
    return 1;
  if (lpc23xx_isp_command (fd, "U 23130") != 0)
    return 1;

  return 0;
}

int
lpc23xx_isp_autobaud_sync (int fd)
{
  if (DumpFlag)
    return 0;

  char buf[64];
  ssize_t sz;
  int retry = 0;

  // Start auto baudrate syncronization.
  fcntl (fd ,F_SETFL, O_NONBLOCK);	// Non-block mode.
  do
    {
      if (retry++)
	{
	  buf[0] = '\r';
	  buf[1] = '\n';
	  write (fd, buf, 2);
	  tcdrain (fd);
	  usleep (100000);
	}

      buf[0] = '?';
      if (write (fd, buf, 1) == -1)
	{
	  perror ("'?' write failed.");
	  return 1;
	}
      tcdrain (fd);

      if ((sz = read (fd, buf, sizeof buf)) == -1)
	{
	  perror ("'?' read failed. PLEASE RESET AGAIN.");
	}
    }
  while (sz != 14);
  fcntl (fd ,F_SETFL, 0);	// Block mode.

  if (strncmp (buf, "Synchronized\r\n", 14) != 0)
    {
      buf[sz] = '\0';
      fprintf (stderr, "Bad responce for '?': '%s' (%zdbyte)\n", buf, sz);
      return 1;
    }
  if ((sz = write (fd, buf, 14)) == -1)
    {
      perror ("Couldn't send reply message.\n");
      return 1;
    }
  if ((sz = read (fd, buf, sizeof buf)) == -1)
    {
      perror ("Couldn't receive 'Synchronized' responce.\n");
      return 1;
    }
  buf[sz] = '\0';
  if (sz != 18 ||
      (strncmp (buf, "Synchronized\r\nOK\r\n", 18) != 0))
    {
      fprintf (stderr, "Bad responce for 'Syncronized': '%s' (%zdbyte)\n",
	       buf, sz);
      return 1;
    }
  fprintf (stderr, "Baudrate synchronized.\n");

  return 0;
}

int
lpc23xx_isp_frequency_set (int fd, int KHz)
{
  if (DumpFlag)
    return 0;

  char buf[64];
  char res[16];
  ssize_t sz;
  int len;

  sprintf (buf, "%d\r\n", KHz);
  sprintf (res, "%d\r\nOK\r\n", KHz);
  if (write (fd, buf, strlen (buf)) == -1)
    {
      perror (__FUNCTION__);
      return 1;
    }
  len = strlen (res);
  if ((sz = read (fd, buf, sizeof buf)) != len || strncmp (buf, res, len) != 0)
    {
      buf[sz] = '\0';
      fprintf (stderr, "%s: bad responce: %s\n", __FUNCTION__, buf);
      return 1;
    }
  fprintf (stderr, "Frequency %d KHz\n", KHz);

  return 0;
}

int
lpc23xx_isp_echo (int fd, int on)
{
  if (DumpFlag)
    return 0;

  static int echo_on = 1;
  char buf[64];
  char res[16];
  size_t sz, len;

  if (echo_on == on)	// no need to change.
    return 0;

  sprintf (buf, "A %d\r\n", on);
  if (on)
    sprintf (res, "0\r\n");
  else
    sprintf (res, "A %d\r\n0\r\n", on);

  if (write (fd, buf, strlen (buf)) == -1)
    {
      perror (__FUNCTION__);
      return 1;
    }
  len = strlen (res);
  if ((sz = read (fd, buf, sizeof buf)) != len || strncmp (buf, res, len) != 0)
    {
      buf[sz] = '\0';
      fprintf (stderr, "%s: bad responce: %s\n", __FUNCTION__, buf);
      return 1;
    }
  echo_on = on;

  fprintf (stderr, "echo %s\n", on ? "on" : "off");

  return 0;
}

int
lpc23xx_isp_command (int fd, const char *cmd)
{
  if (DumpFlag)
    return 0;
  char buf[64];
  ssize_t sz;

  sprintf (buf, "%s\r\n", cmd);
  if (write (fd, buf, strlen (buf)) == -1)
    {
      perror (__FUNCTION__);
      return 1;
    }

  if ((sz = read (fd, buf, sizeof buf)) == -1)
    {
      perror (__FUNCTION__);
      return 1;
    }

  if (strncmp (buf, "0", 1) != 0)
    {
      buf[sz] = '\0';
      fprintf (stderr, "%s: bad responce: %s\n", cmd, buf);
      return 1;
    }
  fprintf (stderr, "%s done.\n", cmd);

  return 0;
}
