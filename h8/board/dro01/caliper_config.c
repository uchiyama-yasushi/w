
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
#include <string.h>
#include <stdlib.h>
#include <reg.h>
#include <sys/delay.h>
#include <sys/console.h>
#include <sys/shell.h>

#include "shan.h"
#include "caliper_config.h"
#include "caliper_terminal.h"
#include "libaki/aki_lcd.h"

STATIC struct caliper_bits shan_bits[SHAN_AXIS_MAX] =
  {
    //data clock  power irq
    { 0x02, 0x01, 0x08, 0x01 },
    { 0x08, 0x04, 0x10, 0x02 },
    { 0x20, 0x10, 0x20, 0x04 },
  };
STATIC struct shan_caliper shan_caliper[SHAN_AXIS_MAX];

#ifdef CALIPER_SHELL_COMMAND
SHELL_COMMAND_DECL (pullup);
#endif

extern struct _file *lcdout;

void
caliper_config_init ()
{

  *P8_DDR = ~0x7;	// Clock Input : 0-2(IN)
  *PB_DDR = 0xff;	// Output (Clock/Data pullup)
  *PA_DDR = 0x38;	// Data Input :0-2(IN), Caliper Power :3-5(OUT)

  *INTC_ISCR = 0x7;	// Falling edge. IRQ0-2
  *INTC_IER = 0x0;	// Disable interrupt. IRQ0-3
  *PA_DR = ~0x38;	// All caliper OFF.
#ifdef CALIPER_SHELL_COMMAND
  shell_command_register (&pullup_cmd);
#endif
}

caliper_t
caliper_config_attach (int axis)
{
  caliper_t c;

  assert (axis >= 0 && axis < 3);

  c = shan_attach (shan_caliper + axis, axis, shan_bits + axis);
  c->pullup_port = PB_DR;
  c->power_port = PA_DR;

  return c;
}

void
caliper_config_outputdata (struct caliper_display_data *display_data)
{
  static int32_t obin[CALIPER_AXIS_MAX];
  static int timer[CALIPER_AXIS_MAX];
  uint8_t lcd_data[2][16];
  uint8_t comm_data[3][8];
  uint8_t *p;
  int display_digit = CALIPER_DIGIT_MAX + 1; // +sign
  uint8_t **bcd = display_data->bcd;
  int i;
  char axis[] = "XYZ";

  memset (lcd_data[0], 0, 32);
  memset (comm_data[0], 0, 8);
  memset (comm_data[1], 0, 8);
  memset (comm_data[2], 0, 8);
  p = lcd_data[0];
  cpu_status_t s = intr_suspend ();
  memcpy (comm_data[0], bcd[0], display_digit);
  memcpy (comm_data[1], bcd[1], display_digit);
  memcpy (comm_data[2], bcd[2], display_digit);
  memcpy (p, bcd[0], display_digit);
  p += display_digit;
  *p++ = ' ';
  memcpy (p, bcd[1], display_digit);
  p = lcd_data[1];
  memcpy (p, bcd[2], display_digit);
  intr_resume (s);

  aki_lcd_line (0);
  fprintf (lcdout, "%s", lcd_data[0]);
  aki_lcd_line (1);
  fprintf (lcdout, "%s", lcd_data[1]);

  for (i = 0; i < CALIPER_AXIS_MAX; i++)
    {
      int32_t bin = display_data->bin[i];
      comm_data[i][0] = axis[i];
      if (bin != obin[i] || timer[i] > 10)
	{
	  iprintf ("D %s\n", comm_data[i]);
	  timer[i] = 0;
	}
      else
	{
	  timer[i]++;
	}

      obin[i] = bin;
    }
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
