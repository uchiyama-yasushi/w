
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

// digital caliper terminal.
// This module is board independent.

#include <sys/system.h>
#include <stdlib.h>
#include <sys/thread.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <string.h>

#include "caliper_terminal.h"
#include "caliper_config.h"

#ifdef CALIPER_SHELL_COMMAND
SHELL_COMMAND_DECL (zero);
SHELL_COMMAND_DECL (dir);
#endif

STATIC void caliper_terminal_zero_soft (int) __attribute ((unused));
STATIC void caliper_terminal_zero_hard (int);
STATIC void caliper_terminal_dir (int);
STATIC void caliper_terminal_bcd_format (uint8_t *, int32_t);

STATIC struct caliper_terminal
{
  int dir;		// increment direction.
  int32_t offset;	// display offset from raw value.
  int32_t raw_data;	// caliper raw data
  uint8_t bcd[8];	// display formated bcd data.
} caliper_terminal[CALIPER_AXIS_MAX];

STATIC struct caliper_display_data __caliper_display_data;

void
caliper_terminal_init ()
{
  struct caliper_terminal *t;
  int i;

#ifdef CALIPER_SHELL_COMMAND
  shell_command_register (&zero_cmd);
  shell_command_register (&dir_cmd);
#endif

  for (i = 0, t = caliper_terminal; i < CALIPER_AXIS_MAX; i++, t++)
    {
      t->offset = 0;
      t->raw_data = 0;
      t->dir = 1;
      __caliper_display_data.bcd[i] = t->bcd;
    }
}

void
caliper_terminal_dispatch_command (uint16_t bit)
{
  int button, axis, command;

  button = ffs (bit) -1;
  axis = TRUNC (button, 4) / 4;
  command = button - axis * 4;

  switch (command)
    {
    case 0:
      iprintf ("power %d\n", axis);
      caliper_manager_power (axis);
      break;
    case 1:
      iprintf ("fast %d\n", axis);
      caliper_manager_fast (axis);
      break;
    case 2:
      iprintf ("dir %d\n", axis);
      caliper_terminal_dir (axis);
      break;
    case 3:
      iprintf ("zero %d\n", axis);
      caliper_terminal_zero_hard (axis);
      caliper_manager_reset (axis);
      break;
    }
}

void
caliper_terminal_zero_soft (int axis)
{
  struct caliper_terminal *t = caliper_terminal + axis;

  t->offset = t->raw_data * t->dir;
}

void
caliper_terminal_zero_hard (int axis)
{
  struct caliper_terminal *t = caliper_terminal + axis;

  t->offset = 0;
}

void
caliper_terminal_dir (int axis)
{
  struct caliper_terminal *t = caliper_terminal + axis;

  t->dir = t->dir  > 0 ? -1 : 1;
  t->offset += t->dir * t->raw_data * 2;
}

void
caliper_terminal_update (int axis, int32_t raw_data)
{

  caliper_terminal[axis].raw_data = raw_data;
  __caliper_display_data.update |= 1 << axis;
}

void
caliper_terminal_display_preprocess ()
{
  int i;
  int32_t tmp;

  for (i = 0; i < CALIPER_AXIS_MAX; i++)
    {
      struct caliper_terminal *t = caliper_terminal + i;
      tmp = t->raw_data * t->dir - t->offset;
      caliper_terminal_bcd_format (t->bcd, tmp);
      __caliper_display_data.bin[i] = tmp;
    }

  caliper_config_outputdata (&__caliper_display_data);
  __caliper_display_data.update = 0;
}

void
caliper_terminal_bcd_format (uint8_t *bcd, int32_t bin)
{
  int i, k, s;
  int32_t j;
  int start = 0;
  int sign_point = 0;

  if ((s = sign (bin)) < 0)
    bin *= -1;

  memset (bcd, BLANK_CHARACTER, 8);
  bcd[7] = '\0';

  for (i = 10000, k = 2; i > 0; i /= 10, k++)
    {
      j = bin / i;
      bin -= j * i;
      if (!start)
	{
	  bcd[k] = BLANK_CHARACTER;
	  if (j)
	    {
	      start = 1;
	      sign_point = k - 1;
	    }
	  if (k > 3)
	    {
	      start = 1;
	      sign_point = k - 1;
	    }
	}

      if (start)
	{
	  bcd[k] = '0' + j;
	}
    }
  if (s < 0)
    bcd[sign_point] = '-';

  return;
}

#ifdef CALIPER_SHELL_COMMAND
// Shell command.
uint32_t
zero (int32_t argc, const char *argv[])
{
  int axis;

  if (argc < 2)
    return -1;
  axis = atoi (argv[1]);

  if (axis < 0 || axis > 3)
    return -1;

  caliper_manager_reset (axis);
  caliper_terminal_zero_hard (axis);

  return 0;
}

uint32_t
dir (int32_t argc, const char *argv[])
{
  int axis;

  if (argc < 2)
    return -1;
  axis = atoi (argv[1]);

  if (axis < 0 || axis > 3)
    return -1;

  caliper_terminal_dir (axis);

  return 0;
}
#endif //CALIPER_SHELL_COMMAND
