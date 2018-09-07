
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
#include <sys/console.h>
#include <sys/shell.h>
#include <sys/board.h>
#include <sys/delay.h>
#include <sys/timer.h>
#include <stdlib.h>
#include <string.h>

#include <reg.h>

#define	LED7_BLANK	0x00

#define	LED7_0		0x3f
#define	LED7_1		0x06
#define	LED7_2		0x5b
#define	LED7_3		0x4f
#define	LED7_4		0x66
#define	LED7_5		0x6d
#define	LED7_6		0x7d
#define	LED7_7		0x07
#define	LED7_8		0x7f
#define	LED7_9		0x67

#define	LED7_DP		0x80
#define	LED7_MINUS	0x40

#define	LED7_A		0x77
#define	LED7_b		0x7c
#define	LED7_c		0x58
#define	LED7_d		0x5e
#define	LED7_E		0x79
#define	LED7_F		0x71
#define	LED7_r		0x50
#define	LED7_o		0x5c

uint8_t digit_pattern[] =
  {
    LED7_0,
    LED7_1,
    LED7_2,
    LED7_3,
    LED7_4,
    LED7_5,
    LED7_6,
    LED7_7,
    LED7_8,
    LED7_9,
    0,
    LED7_E,
  };

uint8_t display_buffer[3][6] = 
  {
    { LED7_MINUS, LED7_MINUS, LED7_MINUS, LED7_MINUS, LED7_MINUS, LED7_MINUS },
    { LED7_MINUS, LED7_MINUS, LED7_MINUS, LED7_MINUS, LED7_MINUS, LED7_MINUS },
    { LED7_MINUS, LED7_MINUS, LED7_MINUS, LED7_MINUS, LED7_MINUS, LED7_MINUS },
  };

struct commons
{
  volatile uint8_t *reg;
  uint8_t bit;
} commons [] = {
  { PDR1, 0x01 },
  { PDR1, 0x02 },
  { PDR1, 0x04 },
  { PDR1, 0x10 },
  { PDR1, 0x20 },
  { PDR1, 0x40 },

  { PDR1, 0x80 },
  { PDR5, 0x01 },
  { PDR5, 0x02 },
  { PDR5, 0x04 },
  { PDR5, 0x08 },
  { PDR5, 0x10 },

  { PDR5, 0x20 },
  { PDR5, 0x40 },
  { PDR5, 0x80 },
  { PDR7, 0x10 },
  { PDR7, 0x20 },
  { PDR7, 0x40 },
};

int conf_sign_flip[3];
//bool conf_debug; RESERVED

uint8_t
poll_ioport_b ()
{
  static uint8_t old_r;
  uint8_t r, e;

  // Jumper switch.
  r = *PDRB;
  conf_sign_flip[0] = !(r & 0x08);	// Open:TRUE, Close FALSE
  conf_sign_flip[1] = !(r & 0x10);
  conf_sign_flip[2] = !(r & 0x20);
  //  conf_debug = !(r & 0x40); RESERVED

  // Push switch.
  r &= 0x7;
  e = (r ^ old_r) & ~r; // Falling edge
  old_r = r;

  return e;
}

void
poll_switch ()
{

  switch (poll_ioport_b ())
    {
    case 0x1:
      iprintf ("z 0\n");	// DRO01 command.
      break;
    case 0x2:
      iprintf ("z 1\n");
      break;
    case 0x4:
      iprintf ("z 2\n");
      break;
    }
}

void
prepare_digit_buffer_led (const char *p)
{
  int sign = 1;
  int i, n, d, j, axis;
  uint8_t *buf;

  axis = *p++ - 'X';
  if (axis < 0 || axis > 2)
    return;

  buf = display_buffer[axis];

  while (*p == ':')
    p++;

  if (*p == '-')
    sign = -1, p++;

  if (conf_sign_flip[axis])
    sign *= -1;

  memset (buf, 0, 6);
  for (n = 0; *p; n++, p++)
    ;
  for (i = j = 0, --p, d = 1; i < n; i++, p--, d *= 10)
    {
      int d = *p - '0';
      if (d < 0 || d > 10)
	d = 11; //Error marker.
      buf[5 - i] = digit_pattern[d];
    }
  buf[3] |= LED7_DP;
  buf[5 - i] = sign < 0 ? LED7_MINUS : 0;
}

void
display ()
{
#define	DELAY_USEC	1000
  int32_t i, j, k;

  for (k = j = 0; k < 3; k++)
    {
      uint8_t *p = display_buffer[k];
      for (i = 0; i < 6; i++, j++)
	{
	  *PDR8 = p[i];
	  *commons[j].reg = commons[j].bit;
	  timer_sleep (DELAY_USEC);
	  *commons[j].reg = 0;
	}
    }
}

void
ioport_init ()
{
  // Port 1
  *PMR1 = PMR1_TXD; // Port 1 I/O port(Disable IRQ, TRGV, TMOW), P22 is TXD.
  *PCR1 = 0xff;	// Output.
  *PUCR1 = 0x0; // No pull-up
  // Port 2 (SCI3)
  *PCR2 = 0x0;	// SCI3
  // Port 5 (I2C)
  *I2C_ICCR = 0; // Disable I2C (P56,P57 are I/O port)
  *PCR5 = 0xff;
  // Port 7 (Timer V)
  *PCR7 = 0xff;
  *TCSRV = 0;	// Timer V/ Disable TMOV (P76 is I/O port)
  // Port 8 (Timer W)
  *PCR8 = 0xff;
  *TMRW = 0;	// Timer W/ Disable FTIOD,FTIOC,FTIOB,FTIOA
  *TCRW = 0;
  *TIERW = 0;
  *TIOR0 = 0;
  *TIOR1 = 0;
  // Port B (ADC)
  *ADC_ADCSR = 7;	// Set ADCSR port (bit 7)

  // Set default.
  *PDR1 = 0;
  *PDR5 = 0;
  *PDR7 = 0;
  *PDR8 = 0;
}

