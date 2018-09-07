
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <dev/usb/usb.h>
#include "h8sxwrite.h"
#include "bootmode_core.h"
#include "bootmode_interface.h"

STATIC int control_pipe;
STATIC int bulkout_pipe;
STATIC int bulkin_pipe;
STATIC bool h8sx_usb_init (int);
STATIC void h8sx_usb_fini (void);
STATIC bool h8sx_match (void);

bool
h8sx_usb_init (int ugen_device_num)
{
  char device_format[] = "/dev/ugen%d.0%d";
  int i;
  char cpipe[16], opipe[16], ipipe[16];

  sprintf (cpipe, device_format, ugen_device_num, 0);
  sprintf (opipe, device_format, ugen_device_num, 1);
  sprintf (ipipe, device_format, ugen_device_num, 2);

  if ((control_pipe = open (cpipe, O_RDWR)) < 0)
    {
      perror ("Can't open control pipe");
      goto error;
    }

  if (!h8sx_match ())
    {
      goto error;
    }

  if ((bulkout_pipe = open (opipe, O_WRONLY)) < 0)
    {
      perror ("Can't open bulk-out pipe");
      goto error;
    }
  if ((bulkin_pipe = open (ipipe, O_RDONLY)) < 0)
    {
      perror ("Can't open bulk-in pipe");
    }

  i = 1;
  if (ioctl (bulkin_pipe, USB_SET_SHORT_XFER, &i) < 0)
    {
      perror ("short xfer failed.\n");
      goto error;
    }
  return TRUE;

 error:
  h8sx_usb_fini ();

  return FALSE;
}

void
h8sx_usb_fini ()
{

  if (bulkout_pipe)
    close (bulkout_pipe);
  if (bulkin_pipe)
    close (bulkin_pipe);
  if (control_pipe)
    close (control_pipe);
}

bool
h8sx_bootprog_init (int ugen_device_n, struct board_config *board_config,
		    struct flash_config *flash_config)
{

  if (!h8sx_usb_init (ugen_device_n))
    return FALSE;

  if (!initialize (bulkout_pipe, bulkin_pipe) || !check_status (DEVICE_SELECT))
    goto error;

  // Inquire Chip infomation.
  if (!support_device_inquire (flash_config) ||
      !clock_inquire (flash_config) ||
      !clock_mode_inquire (flash_config) ||
      !multiplication_ratio_inquire (flash_config) ||
      !userboot_mat_inquire (flash_config) ||
      !user_mat_inquire (flash_config) ||
      !erased_block_inquire (flash_config) ||
      !write_size_inquire (flash_config))
    goto error;

  // Configure boot program.
  if (!device_select (flash_config->device_code))
    goto error;
  DPRINTF ("Device select done.\n");

  if (!clock_mode_select (board_config->MD_CLK))
    goto error;
  DPRINTF ("Clock select done.\n");

  if (!new_bitrate_select (flash_config, board_config->input_frequency))
    goto error;
  DPRINTF ("New bitrate select done.\n");

  if (!transition_write_erase_state ())
    goto error;
  DPRINTF ("Erase done. Write/Erase mode.\n");

  if (!user_mat_select ())
    goto error;
  DPRINTF ("User MAT ready to write.\n");

 error:

  return TRUE;
}

void
h8sx_bootprog_fini (struct flash_config *config)
{

  if (config->clock_mode)
    free (config->clock_mode);

  if (config->userboot_mat)
    free (config->userboot_mat);

  if (config->user_mat)
    free (config->user_mat);
}

bool
h8sx_bootprog_write (struct flash_config *config, uint8_t *data, size_t data_sz)
{
  size_t write_size = config->write_size;
  addr_t addr;
  int nblock;
  int i;
  bool success = FALSE;

  nblock = ROUND (data_sz, write_size) / write_size;
  addr = config->user_mat[0].min;

  // Write image to flash memory.
  for (i = 0; i < nblock; i++, data += write_size, addr += write_size)
    {
      if (!write_data (addr, data))
	break;
    }
  if (i != nblock)
    DPRINTF ("*** Write failed.***\n");
  else
    success = TRUE;

  write_end ();

  //  if (!read_data (0, 2048))
  //    goto write_error;
  //  DPRINTF ("Read done.\n");

  h8sx_usb_fini ();

  return success;
}

STATIC bool
h8sx_match ()
{
  struct usb_device_info udi;

  if (ioctl (control_pipe, USB_GET_DEVICEINFO, &udi) < 0)
    {
      perror ("can't get device information");
      return FALSE;
    }
  fprintf (stderr, "vendor: %04x product: %04x\n", udi.udi_vendorNo,
	   udi.udi_productNo);

  if (udi.udi_vendorNo != 0x045b || udi.udi_productNo != 0x0025)
    {
      fprintf(stderr, "Not boot mode of H8SX/1655\n");
      return FALSE;
    }

  return TRUE;
}


