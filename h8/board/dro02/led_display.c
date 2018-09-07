
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

// 6 digit 4 axis 7segment LED display unit.

#include <sys/system.h>
#include <stdlib.h>

#include <sys/delay.h>
#include <reg.h>
#include <sys/console.h>

#include <sys/shell.h>

#include "led_display.h"

#ifdef CALIPER_SHELL_COMMAND
SHELL_COMMAND_DECL (led_test);
#endif

STATIC uint8_t __7segled_data[2][CALIPER_DIGIT_MAX];

void
led_display_init ()
{

  *P4_DDR = 0xf0;	// P4 [4-7] output. Select cathode common.
  *P3_DDR = 0xff;
  *P1_DDR = 0xff;

  *P3_DR = 0xff;	// caliper 0 | caliper 1 bcd
  *P1_DR = 0xff;	// caliper 2 | caliper 3 bcd

  // Display test.
  int i, j;
  for (i = 0; i < 100; i++)
    {
      for (j = 0; j < CALIPER_DIGIT_MAX; j++)
	{
	  uint8_t r= j | (j << 4);
	  *P3_DR = r;
	  *P1_DR = r;
	  *P4_DR = j << 4;
	  mdelay (1);
	}
    }

  // Set blank.
  *P3_DR = 0xaa;	// blank
  *P1_DR = 0xaa;	// blank
  for (j = 0; j < CALIPER_DIGIT_MAX; j++)
    *P4_DR = j << 4;

#ifdef CALIPER_SHELL_COMMAND
  shell_command_register (&led_test_cmd);
#endif
}

void
led_display_set_data (uint8_t data[CALIPER_AXIS_MAX][CALIPER_DIGIT_MAX])
{
  int i;

  // Convert to 7seg LED display format.
  for (i = 0; i < CALIPER_DIGIT_MAX; i++)
    {
      __7segled_data[0][i] =
	(data[0][i] & 0x0f) | ((data[1][i] << 4) & 0xf0);
      __7segled_data[1][i] =
	(data[2][i] & 0x0f) | ((data[3][i] << 4) & 0xf0);
    }
}

void
led_display_output ()
{

  int i;
  for (i = 0; i < CALIPER_DIGIT_MAX; i++)
    {
      *P1_DR = __7segled_data[1][i];
      *P3_DR = __7segled_data[0][i];
      *P4_DR = i << 4;
      // 10 .. NG
      // 3 ..OK fast mode blinking.
      // 2 ..OK fast mode better than 3.
      // 1...OK
      mdelay (2);
    }
}

#ifdef CALIPER_SHELL_COMMAND
uint32_t
led_test (int32_t argc, const char *argv[])
{
  int i, t, n;

  for (i = 0; i < argc; i++)
    printf ("[%d] %s\n", i, argv[i]);
  if (argc < 3)
    return 0;

  t = atoi (argv[1]);
  n = atoi (argv[2]);

  printf ("delay=%d loop=%d\n", t, n);

  for (i = 0; i < n; i++)
    {
      caliper_terminal_preprocess ();
      led_display_output (caliper_terminal_7segled_data, t);
    }

  return 0;
}
#endif // CALIPER_SHELL_COMMAND
