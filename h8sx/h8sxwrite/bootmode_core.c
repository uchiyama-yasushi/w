
/*-
 * Copyright (c) 2010 UCHIYAMA Yasushi.  All rights reserved.
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

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "h8sxwrite.h"
#include "bootmode_core.h"

STATIC bool check_new_bitrate (void);
STATIC uint8_t send_command (uint8_t *, ssize_t);
STATIC void boot_program_status_print (uint8_t);
STATIC void boot_program_error_print (uint8_t);

STATIC int bulkout_pipe_;
STATIC int bulkin_pipe_;
STATIC uint8_t transfer_buf_[4096];

// Start boot program.
bool
initialize (int bulkout_pipe, int bulkin_pipe)
{
  uint8_t a = 0x55;

  bulkout_pipe_ = bulkout_pipe;
  bulkin_pipe_ = bulkin_pipe;

  if (write (bulkout_pipe, &a, 1) != 1)
    {
      fprintf (stderr, "0x55 failed\n");
      return FALSE;
    }
  a = 0;
  if (read (bulkin_pipe, &a, 1) != 1)
    {
      fprintf (stderr, "no 0xx55 response\n");
      return FALSE;
    }

  if (a != 0xe6)
    {
      fprintf (stderr, "Invalid boot program response: %x\n", a);
      return FALSE;
    }

  return TRUE;
}

// Get current boot program status.
bool
check_status (enum boot_program_state state)
{
  struct boot_program_status_response *p;

  if (!(p = inquire (BOOT_PROGRAM_STATUS_INQUIRE)))
    return FALSE;

  boot_program_status_print (p->status);
  boot_program_error_print (p->error);

  if (p->error)
    return FALSE;
  if (p->status != state)
    {
      fprintf (stderr, "current status:%x, required status:%x\n",
	       p->status, state);
      return FALSE;
    }

  return TRUE;
}

// Issue boot program command
bool
command (uint8_t *cmd, ssize_t sz, enum boot_program_state next_state)
{

  if (send_command (cmd, sz) != 0)
    {
      return FALSE;
    }

  if (cmd[0] == BITRATE_SELECT_COMMAND && !check_new_bitrate ())
    {
      return FALSE;
    }

  return check_status (next_state);
}

// Issue inquiry command.
void *
inquire (uint8_t command)
{
  uint8_t response, checksum;
  ssize_t sz, data_sz;
  int i;

  memset (transfer_buf_, 0, sizeof transfer_buf_);
  DPRINTF ("Command %x: ", command);

  if (write (bulkout_pipe_, &command, 1) != 1)
    {
      perror ("Inquire issue failed");
      return 0;
    }

  if ((sz = read (bulkin_pipe_, transfer_buf_, sizeof transfer_buf_)) == -1)
    {
      perror ("Inquire didn't respond");
      return 0;
    }

  if ((response = transfer_buf_[0]) != command + 0x10)
    {
      fprintf (stderr, "Invalid response %x. commnad = %x", response, command);
      return 0;
    }
  if (command == ERASED_BLOCK_INQUIRE)
    // +3 are response(1byte) + size(2byte)
    data_sz = (transfer_buf_[1] << 8) | (transfer_buf_[2] + 3);
  else
    // +2 are response(1byte) + size(1byte)
    data_sz = transfer_buf_[1] + 2;
  if (data_sz != sz)
    {
      fprintf (stderr, "Mismatch data size. %ld != %ld\n", data_sz + 2, sz);
      return 0;
    }

  DPRINTF ("read %ldbyte\n", sz);
  DPRINTF ("{ ");
  for (i = 0, checksum = 0; i < sz; i++)
    {
      checksum += transfer_buf_[i];
      DPRINTF ("0x%x, ", transfer_buf_[i]);
    }
  DPRINTF ("}\n");

  if (read (bulkin_pipe_, transfer_buf_, 1) < 0)
    {
      perror ("Can't read checksum");
      return 0;
    }

  if ((checksum += transfer_buf_[0]) != 0)
    {
      perror ("Checksum error");
      return 0;
    }

  return transfer_buf_;
}

STATIC uint8_t
send_command (uint8_t *cmd, ssize_t cmd_sz)
{
  ssize_t sz;

  if (write (bulkout_pipe_, cmd, cmd_sz) != cmd_sz)
    {
      perror ("Command issue failed");
      return 0;
    }

  if ((sz = read (bulkin_pipe_, transfer_buf_, sizeof transfer_buf_)) == -1)
    {
      perror ("Command didn't respond");
      return 0;
    }

  if (transfer_buf_[0] == 0x06)	// Success.
    return 0;

  if (transfer_buf_[0] != cmd[0] + 0x80)
    {
      fprintf (stderr, "Invalid error response. %x != %x\n", transfer_buf_[0], 
	       cmd[0] + 0x80);
    }

  DPRINTF ("Error response:%02x, %02x\n", transfer_buf_[0], transfer_buf_[1]);
  boot_program_error_print (transfer_buf_[1]);

  return transfer_buf_[1];
}

// Special routine for new bitrate select.
STATIC bool
check_new_bitrate ()
{
  uint8_t command = 0x06;
  ssize_t sz;

  if (write (bulkout_pipe_, &command, 1) != 1)
    {
      perror ("Couldn't send new bitrate ACK.");
      return FALSE;
    }
  if ((sz = read (bulkin_pipe_, transfer_buf_, sizeof transfer_buf_)) == -1)
    {
      perror ("Boot program don't respond new bitrate.");
      return FALSE;
    }

  if (transfer_buf_[0] != 0x06)
    {
      fprintf (stderr, "Invalid new bitrate response. %x != 0x06\n",
	       transfer_buf_[0]);
      return FALSE;
    }

  return TRUE;
}


STATIC void
boot_program_status_print (uint8_t status_code)
{

  switch (status_code)
    {
    default:
      fprintf (stderr, "Unknown status code: %d\n", status_code);
      break;
    case 0x11:
      DPRINTF ("Device selection wait\n");
      break;
    case 0x12:
      DPRINTF ("Clock mode selection wait\n");
      break;
    case 0x13:
      DPRINTF ("Bit rate selection wait\n");
      break;
    case 0x1F:
      DPRINTF ("Programming/erasing state transition wait "
	      "(bit rate selection is completed)\n");
      break;
    case 0x31:
      DPRINTF ("Programming state for erasure\n");
      break;
    case 0x3F:
      DPRINTF ("Programming/erasing selection wait\n");
      break;
    case 0x4F:
      DPRINTF ("Program data receive wait\n");
      break;
    case 0x5F:
      DPRINTF ("Erase block specification wait\n");
      break;
    }
}

STATIC void
boot_program_error_print (uint8_t error_code)
{
  if (!error_code)
    return;

  switch (error_code)
    {
    default:
      fprintf (stderr, "Unknown error code: %d\n", error_code);
      break;
    case 0x11:
      fprintf (stderr, "Sum check error\n");
      break;
    case 0x12:
      fprintf (stderr, "Program size error\n");
      break;
    case 0x21:
      fprintf (stderr, "Device code mismatch error\n");
      break;
    case 0x22:
      fprintf (stderr, "Clock mode mismatch error\n");
      break;
    case 0x24:
      fprintf (stderr, "Bit rate selection error\n");
      break;
    case 0x25:
      fprintf (stderr, "Input frequency error\n");
      break;
    case 0x26:
      fprintf (stderr, "Multiplication ratio error\n");
      break;
    case 0x27:
      fprintf (stderr, "Operating frequency error\n");
      break;
    case 0x29:
      fprintf (stderr, "Block number error\n");
      break;
    case 0x2A:
      fprintf (stderr, "Address error\n");
      break;
    case 0x2B:
      fprintf (stderr, "Data length error\n");
      break;
    case 0x51:
      fprintf (stderr, "Erasure error\n");
      break;
    case 0x52:
      fprintf (stderr, "Erasure incomplete error\n");
      break;
    case 0x53:
      fprintf (stderr, "Programming error\n");
      break;
    case 0x54:
      fprintf (stderr, "Selection processing error\n");
      break;
    case 0x80:
      fprintf (stderr, "Command error\n");
      break;
    case 0xFF:
      fprintf (stderr, "Bit-rate-adjustment confirmation error\n");
      break;
    }

  return;
}
