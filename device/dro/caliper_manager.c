
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

// digital caliper manager. output canonical data.
// This module is caliper type independent.
#include <sys/system.h>
#include <stdlib.h>
#include <string.h>

#include <sys/thread.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <sys/delay.h>

#include "caliper_manager.h"
#include "caliper_terminal.h"
#include "caliper_config.h"

#ifdef CALIPER_SHELL_COMMAND
SHELL_COMMAND_DECL (caliper_power);
SHELL_COMMAND_DECL (caliper_reset);
SHELL_COMMAND_DECL (caliper_fast);
#endif

thread_t caliper_manager_thread;
// Global for interrupt handler.
caliper_t __caliper[CALIPER_AXIS_MAX];

void
caliper_manager (uint32_t arg __attribute__((unused)))
{
  int i;

  intr_disable ();
  caliper_manager_thread = current_thread;
  iprintf ("caliper_manager_thread=%p\n", current_thread);
  thread_priority (current_thread, PRI_HIGH); // This is interrupt thread.
#ifdef CALIPER_SHELL_COMMAND
  shell_command_register (&caliper_power_cmd);
  shell_command_register (&caliper_reset_cmd);
  shell_command_register (&caliper_fast_cmd);
#endif
  // Setup board specific configuration.
  caliper_config_init ();

  // Terminal
  caliper_terminal_init ();

  // Attach caliper.
  for (i = 0; i < CALIPER_AXIS_MAX; i++)
    __caliper[i] = caliper_config_attach (i);

  // Intialize caliper.
  iprintf ("caliper initialize...");
  for (i = 0; i < CALIPER_AXIS_MAX; i++)
    {
      caliper_t c = __caliper[i];
      c->axis = i;
      c->power = FALSE;
      c->fast_mode = FALSE;
      c->ops.init (__caliper[i]);
    }
  iprintf ("done.\n");

  intr_enable ();
  // Data sampling loop.
  while (/*CONSTCOND*/1)
    {
      thread_block (NULL);
      for (i = 0; i < CALIPER_AXIS_MAX; i++)
	{
	  caliper_t c = __caliper[i];
	  // Extract data from interrupt handler.
	  if (c->ops.update (c))
	    {
	      // Push to terminal.
	      caliper_terminal_update (c->axis, c->raw_data);
	    }
	}
      // Setup new display data.
      caliper_terminal_display_preprocess ();
    }
}

void
caliper_manager_reset (int axis)
{

  __caliper[axis]->ops.reset (__caliper[axis]);
}

void
caliper_manager_fast (int axis)
{

  __caliper[axis]->ops.fast (__caliper[axis]);
}

void
caliper_manager_power (int axis)
{

  __caliper[axis]->ops.power (__caliper[axis]);
}


#ifdef CALIPER_SHELL_COMMAND
uint32_t
caliper_power (int32_t argc, const char *argv[])
{
  int axis;

  if (argc < 2)
    return -1;
  axis = atoi (argv[1]);

  if (axis < 0 || axis > 3)
    return -1;

  __caliper[axis]->ops.power (__caliper[axis]);

  return 0;
}

uint32_t
caliper_reset (int32_t argc, const char *argv[])
{
  int axis;

  if (argc < 2)
    {
      for (axis = 0; axis < CALIPER_AXIS_MAX; axis++)
	__caliper[axis]->ops.reset (__caliper[axis]);
      return 0;
    }

  axis = atoi (argv[1]);
  if (axis < 0 || axis > 3)
    return -1;

  __caliper[axis]->ops.reset (__caliper[axis]);

  return 0;
}

uint32_t
caliper_fast (int32_t argc, const char *argv[])
{
  int axis;

  if (argc < 2)
    {
      for (axis = 0; axis < CALIPER_AXIS_MAX; axis++)
	__caliper[axis]->ops.fast (__caliper[axis]);
      return 0;
    }

  axis = atoi (argv[1]);

  if (axis < 0 || axis > 3)
    return -1;

  __caliper[axis]->ops.fast (__caliper[axis]);
  return 0;
}
#endif // CALIPER_SHELL_COMMAND
