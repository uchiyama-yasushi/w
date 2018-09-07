
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
#include <stdlib.h>
#include <assert.h>

#include "h8sxwrite.h"
#include "bootmode_core.h"
#include "bootmode_interface.h"

STATIC uint8_t calcurate_checksum (uint8_t *, ssize_t);

bool
support_device_inquire (struct flash_config *config)
{
  struct support_device_response *p;

  if (!(p = inquire (SUPPORT_DEVICE_INQUIRE)))
    return FALSE;

  strncpy (config->device_code, p->device_code, 4);
  VPRINTF ("Boot program name: %s, # of supported devices: %d\n",
	  p->boot_prog_name, p->n_device);
  VPRINTF ("Device code: %s\n", config->device_code);

  return TRUE;
}

bool
clock_inquire (struct flash_config *config)
{
  struct clock_response *p;
  int i;

  if (!(p = inquire (CLOCK_INQUIRE)))
    return FALSE;

  VPRINTF ("# of source clocks: %d\n", p->n_src_clock);

  config->n_clock_range = p->n_src_clock;
  config->clock_range = malloc (sizeof (struct minmax) * p->n_src_clock);
  assert (config->clock_range);

  for (i = 0; i < p->n_src_clock; i++)
    {
      config->clock_range[i].min = GET16 (p->clock[i].min);
      config->clock_range[i].max = GET16 (p->clock[i].max);
      VPRINTF ("(%.2fMHz-%.2fMHz)\n",
	      (float)config->clock_range[i].min / 100.,
	      (float)config->clock_range[i].max / 100.);
    }

  return TRUE;
}

bool
clock_mode_inquire (struct flash_config *config)
{
  struct clock_mode_response *p;
  int i;

  if (!(p = inquire (CLOCK_MODE_INQUIRE)))
    return FALSE;

  config->clock_mode = malloc (p->size);
  assert (config->clock_mode);

  config->n_clock_mode = p->size;
  memcpy (config->clock_mode, p->mode, p->size);

  VPRINTF ("Clock mode: ");
  for (i = 0; i < p->size; i++)
    {
      VPRINTF ("0x%02x, ", config->clock_mode[i]);
    }
  VPRINTF ("\n");

  return TRUE;
}

bool
multiplication_ratio_inquire (struct flash_config *config)
{
  struct multiplication_ratio_response *p;
  int i;

  if (!(p = inquire (MULTIPLICATION_RATIO_INQUIRE)))
    return FALSE;

  VPRINTF ("# of types of multiplication: %d, # of multiplication ratios: %d\n",
	  p->n_mult_type, p->n_ratio);
  VPRINTF ("Multiplication ratio: ");
  for (i = 0; i < p->n_ratio; i++)
    {
      VPRINTF ("%d, ", (int)p->ratio[i]);
    }
  VPRINTF ("\n");
  VPRINTF ("Type of multiplication: ");
  for (i = 0; i < p->n_mult_type; i++)
    {
      VPRINTF ("0x%02x, ", p->ratio[i + p->n_ratio]);
    }
  VPRINTF ("\n");

  config->n_mult_type = p->n_mult_type;

  return TRUE;
}

bool
userboot_mat_inquire (struct flash_config *config)
{
  struct mat_response *p;
  int i;

  if (!(p = inquire (USERBOOT_MAT_INQUIRE)))
    return FALSE;

  config->n_userboot_mat = p->n_area;
  config->userboot_mat = malloc (sizeof (struct minmax) * p->n_area);
  assert (config->userboot_mat);

  VPRINTF ("# of %s MAT areas: %d\n", "User Boot", p->n_area);
  for (i = 0; i < p->n_area; i++)
    {
      config->userboot_mat[i].min = GET32 (p->address[i].start);
      config->userboot_mat[i].max = GET32 (p->address[i].end);
      VPRINTF ("0x%08x-0x%08x\n",
	      config->userboot_mat[i].min,
	      config->userboot_mat[i].max);
    }

  return TRUE;
}

bool
user_mat_inquire (struct flash_config *config)
{
  struct mat_response *p;
  int i;

  if (!(p = inquire (USER_MAT_INQUIRE)))
    return FALSE;

  config->n_user_mat = p->n_area;
  config->user_mat = malloc (sizeof (struct minmax) * p->n_area);
  assert (config->user_mat);

  VPRINTF ("# of %s MAT areas: %d\n", "User", p->n_area);
  for (i = 0; i < p->n_area; i++)
    {
      config->user_mat[i].min = GET32 (p->address[i].start);
      config->user_mat[i].max = GET32 (p->address[i].end);
      VPRINTF ("0x%08x-0x%08x\n",
	      config->user_mat[i].min,
	      config->user_mat[i].max);
    }

  return TRUE;
}

bool
erased_block_inquire (struct flash_config *config __attribute__((unused)))
{
  struct erased_block_response *p;
  int i;

  if (!(p = inquire (ERASED_BLOCK_INQUIRE)))
    return FALSE;

  VPRINTF ("# of erased blocks: %d\n", p->n_block);
  for (i = 0; i < p->n_block; i++)
    {
      p->block[i].start = GET32 (p->block[i].start);
      p->block[i].end = GET32 (p->block[i].end);
      VPRINTF ("(0x%08x-0x%08x\n", p->block[i].start, p->block[i].end);
    }

  return TRUE;
}

bool
write_size_inquire (struct flash_config *config)
{
  struct write_size_response *p;

  if (!(p = inquire (WRITE_SIZE_INQUIRE)))
    return FALSE;

  config->write_size = GET16 (p->write_size);
  VPRINTF ("write size: %ldbyte\n", config->write_size);

  return TRUE;
}

bool
device_select (const char *device_code)
{
  struct device_select_command cmd;

  if (!check_status (DEVICE_SELECT))
    return FALSE;

  cmd.command = DEVICE_SELECT_COMMAND;
  cmd.size = 4;
  memcpy (cmd.device_code, device_code, 4);
  cmd.checksum = calcurate_checksum ((uint8_t *)&cmd, sizeof cmd);

  return command ((uint8_t *)&cmd, sizeof cmd, CLOCK_MODE_SELECT);
}

bool
clock_mode_select (uint8_t mode)
{
  struct clock_mode_select_command cmd;

  if (!check_status (CLOCK_MODE_SELECT))
    return FALSE;

  cmd.command = CLOCK_MODE_SELECT_COMMAND;
  cmd.size = 1;
  cmd.mode = mode;
  cmd.checksum = calcurate_checksum ((uint8_t *)&cmd, sizeof cmd);

  return command ((uint8_t *)&cmd, sizeof cmd, BITRATE_SELECT);
}

bool
new_bitrate_select (struct flash_config *flash_config,
		    uint8_t input_frequency/*MHz*/)
{
  struct bitrate_select_command cmd;

  if (!check_status (BITRATE_SELECT))
    return FALSE;

  flash_config->n_mult_type = 2;

  cmd.command = BITRATE_SELECT_COMMAND;
  cmd.size = 2 + 2 + 1 + flash_config->n_mult_type;
  cmd.bitrate = SET16 (0x00c0);
  cmd.clock = SET16 (input_frequency * 100);
  cmd.n_mult_type = flash_config->n_mult_type;
  cmd.main_clock_ratio = 4;	//XXX
  cmd.peripheral_clock_ratio = 2;	//XXX
  cmd.checksum = calcurate_checksum ((uint8_t *)&cmd, sizeof cmd);

  return command ((uint8_t *)&cmd, sizeof cmd, STATE_TRANSITION);
}

bool
transition_write_erase_state ()
{
  uint8_t cmd = TRANSITION_WRITE_ERASE_STATE_COMMAND;

  return command (&cmd, 1, WRITE_ERASE_SELECT_WAIT);
}

bool
user_mat_select ()
{
  uint8_t cmd = USER_MAT_SELECT_COMMAND;

  return command (&cmd, 1, DATA_WAIT);
}

bool
write_data (addr_t addr, uint8_t *buf)
{
  struct write_128byte_command cmd;

  DPRINTF ("%s: ADDR=%x\n", __FUNCTION__, addr);

  cmd.command = WRITE_128BYTE_COMMAND;
  cmd.addr = SET32 (addr);
  memcpy (cmd.data, buf, 1024); //XXX
  cmd.checksum = calcurate_checksum ((uint8_t *)&cmd, sizeof cmd);

  return command ((uint8_t *)&cmd, sizeof cmd, DATA_WAIT);
}

#if 0 //notyet
bool
read_data (uint32_t addr, uint32_t size)
{
  struct read_command cmd;
  struct read_command_response *p;
  ssize_t sz;
  uint32_t i;

  cmd.command = READ_COMMAND;
  cmd.size = 9;
  cmd.area = 1;	// User MAT
  cmd.addr = SET32 (addr);
  cmd.read_size = SET32 (size);
  cmd.checksum = calcurate_checksum ((uint8_t *)&cmd, sizeof cmd);

  if (write (bulkout_pipe, &cmd, sizeof cmd) != sizeof cmd)
    {
      perror ("Command(0x52) issue failed");
      return FALSE;
    }

  if ((sz = read (bulkin_pipe, buf, sizeof buf)) == -1)
    {
      perror ("Command(0x52) didn't respond");
      return 0;
    }

  p = (struct read_command_response *)buf;
  if (p->response != READ_COMMAND)
    {
      fprintf (stderr, "Command(0x52) failed.\n");
      boot_program_error_print (buf[1]);
      return FALSE;
    }
  printf ("response=%x addr=%x\n", p->response, p->addr);
  for (i = 0; i < size; i++)
    {
      printf ("(%d:%x) ", i, p->data[i]);
    }
  printf ("\n");

  return TRUE;
}
#endif

bool
write_end ()
{
  struct write_end_command cmd;

  cmd.command = WRITE_END_COMMAND;
  cmd.addr = 0xffffffff;
  cmd.checksum = calcurate_checksum ((uint8_t *)&cmd, sizeof cmd);

  return command ((uint8_t *)&cmd, sizeof cmd, WRITE_ERASE_SELECT_WAIT);
}

uint8_t
calcurate_checksum (uint8_t *p, ssize_t sz)
{
  uint8_t checksum = 0;
  int i;

  for (i = 0; i < sz - 1; i++)
    checksum += *p++;

  return -checksum;
}
