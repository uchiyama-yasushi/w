
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

// DRO board depenednt implementation.

#include <sys/system.h>
#include <stdlib.h>
#include <reg.h>
#include <sys/delay.h>
#include <sys/console.h>
#include <sys/shell.h>

#include "isagida.h"
#include "shan.h"
#include "caliper_config.h"
#include "caliper_terminal.h"
#include "led_display.h"

STATIC struct caliper_bits h8_port_bits[CALIPER_AXIS_MAX] =
  {
    //data clock  power irq
    { 0x02, 0x01, 0x01, 0x01 },
    { 0x08, 0x04, 0x02, 0x02 },
    { 0x20, 0x10, 0x04, 0x04 },
    { 0x80, 0x40, 0x08, 0x08 },
  };
STATIC struct shan_caliper shan_caliper[SHAN_AXIS_MAX];
STATIC struct isagida_caliper isagida_caliper[ISAGIDA_AXIS_MAX];

#ifdef CALIPER_SHELL_COMMAND
SHELL_COMMAND_DECL (pullup);
#endif

void
caliper_config_init ()
{

  *P8_DDR = 0;		// Clock Input (Data input: PA[4:7])
  *PB_DDR = 0xff;	// Output (Clock/Data pullup)
  *PA_DDR = 0x0f;	// Output (Caliper Power[0:3])/ Data Input [4:7]

  *INTC_ISCR = 0xf;	// Falling edge. IRQ0-3
  *INTC_IER = 0x0;	// Disable interrupt. IRQ0-3
  *PA_DR = 0x0f;	// All caliper OFF.
#ifdef CALIPER_SHELL_COMMAND
  shell_command_register (&pullup_cmd);
#endif
}

caliper_t
caliper_config_attach (int axis)
{
  caliper_t c;

  if (axis < 3)
    {
      c = shan_attach (shan_caliper + axis, axis, h8_port_bits + axis);
    }
  else
    {
      c = isagida_attach (isagida_caliper, axis, h8_port_bits + axis);
    }

  c->pullup_port = PB_DR;
  c->power_port = PA_DR;

  return c;
}

void
caliper_config_outputdata (struct caliper_display_data *display_data)
{
  uint8_t data[CALIPER_AXIS_MAX][CALIPER_DIGIT_MAX];
  int i, j;

  for (i = 0; i < CALIPER_AXIS_MAX; i++)
    {
      uint8_t *d = data[i];
      uint8_t *bcd = display_data->bcd[i];
      //      iprintf ("%s\n", bcd);
      for (j = CALIPER_DIGIT_MAX - 1; j >= 0; j--)
	d[j] = bcd[CALIPER_DIGIT_MAX - j] - '0';
    }
  led_display_set_data (data);
}


#ifdef CALIPER_SHELL_COMMAND
uint32_t
pullup (int32_t argc, const char *argv[])
{
  int i, bits, delay_msec;

  for (i = 0; i < argc; i++)
    printf ("[%d] %s\n", i, argv[i]);
  if (argc < 3)
    goto failed;
  bits = atoi (argv[1]) & 0xff;
  delay_msec = atoi (argv[2]);

  printf ("%x %d\n", bits, delay_msec);

  cpu_status_t s = intr_suspend ();
  *PB_DR |= bits;
  mdelay (delay_msec);
  *PB_DR &= ~bits;
  intr_resume (s);

  return 0;
 failed:
  printf ("invalid parameter\n");
  return -1;
}
#endif // CALIPER_SHELL_COMMAND
