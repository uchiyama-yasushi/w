
/*-
 * Copyright (c) 2008 UCHIYAMA Yasushi.  All rights reserved.
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

/* Logic analize applications with UD01 on-board LM339N */
/* LM339N inverted amplifier, full swing to 3.3V */

#include <sys/system.h>
#include <sys/thread.h>
#include <sys/console.h>
#include <string.h>

#include <reg.h>
#include <ud01/ud01dev.h>

// for rfshc_cmi_ud01.S
#define	MAX_SAMPLE	256
uint8_t la_sample_data[MAX_SAMPLE];
int32_t la_sample_cnt;

// for port_polling.S
#define	MAX_POLL_SAMPLE	128
int32_t la_high[MAX_POLL_SAMPLE], la_low[MAX_POLL_SAMPLE];

void lcd_data_write (int, uint8_t);
void port_polling (volatile uint8_t *, int8_t, int32_t);

// for irq5_ud01.S
uint16_t la_t0;

void
la_100KHz ()
{
  int i;

  intr_disable ();
  iprintf ("sampling start\n");
  port_polling (P8_DR, 3, MAX_POLL_SAMPLE); // Poll I/O Port 8-3
  iprintf ("sampling end\n");
  for (i = 0; i < MAX_POLL_SAMPLE - 1; i++)
    {
      iprintf ("H %d\n", la_high[i]);
      iprintf ("L %d\n", la_low[i + 1]);
    }
  while (/*CONSTCOND*/1)
    ;
  /* NOTREACHED */

  // Sampling IRQ5 interval with interrupt. (use irq5_itu0.S and itu0_ovf.S)
  // check board_intc_conf.h
  *ITU_TSTR |= ITU_TSTR_STR0;	// timer 0 start
  // IRQ5
  *INTC_ISCR = 0x20;		// Falling edge. IRQ5
  *INTC_IER = 0x20;
  intr_enable ();

  while (/*CONSTCOND*/1)
    {
      iprintf ("%d\n", la_t0);
    }
}

void
la_10KHz ()
{
  // Use refresh controller as interval timer.
  *RFSHC_RFSHCR = RFSHCR_INTERVAL_TIMER;
  *RFSHC_RTCNT = 0x00;
  *RFSHC_RTCOR = 0xff;

  *INTC_IER = 0x00;
  *P9_DDR = ~0x30;	// 4, 5 Input.

  memset (la_sample_data, 0, MAX_SAMPLE);
  la_sample_cnt = 0;
  while (1)
    {
      int8_t c = getc (SERIAL);
      switch (c)
	{
	case '0':
	  *RFSHC_RTMCSR = RTMCSR_CLK_2 | RTMCSR_CMIE;
	  *RFSHC_RTCOR = 0xff;
	  break;
	case '1':
	  *RFSHC_RTMCSR = RTMCSR_CLK_8 | RTMCSR_CMIE;
	  *RFSHC_RTCOR = 0xff;
	  break;
	case '2':
	  *RFSHC_RTMCSR = RTMCSR_CLK_32 | RTMCSR_CMIE;
	  break;
	case '3':
	  *RFSHC_RTMCSR = RTMCSR_CLK_128 | RTMCSR_CMIE;
	  break;
	case '4':
	  *RFSHC_RTMCSR = RTMCSR_CLK_512 | RTMCSR_CMIE;
	  break;
	case '5':
	  *RFSHC_RTMCSR = RTMCSR_CLK_2048 | RTMCSR_CMIE;
	  break;
	case '6':
	  *RFSHC_RTMCSR = RTMCSR_CLK_4096 | RTMCSR_CMIE;
	  break;
	}
      while (*RFSHC_RTMCSR & RTMCSR_CMIE)
	;
      lcd_clear (0);
      lcd_data_write (0, 0x20);
      lcd_data_write (1, 0x10);
      memset (la_sample_data, 0, MAX_SAMPLE);
      la_sample_cnt = 0;
    }
}

void
lcd_data_write (int channel, uint8_t bitmask)
{
  int low_y[2] = { 14, 30 };
  int high_y[2] = { 2, 18 };
  int start[2] = { 0, 122 };
  uint8_t r, or;
  int high_cnt, low_cnt;
  int i, k;
  int low = low_y[channel];
  int high = high_y[channel];

  or = 0, high_cnt = 0, low_cnt = 0;
  for (i = start[channel], k = 0; i < start[channel] + 122; i++, k++)
    {
      r = la_sample_data[i] & bitmask;
      if ((r ^ or) & ~r) // Falling edge
	{
	  printf ("high %d %d ", high_cnt, low_cnt);
	  printf ("%dHz\n", 10000000/((low_cnt + high_cnt) * 204));
	  low_cnt = 0;
	  lcd_line (k, low, k, high);
	}
      if ((r ^ or) & r) // Rising edge
	{
	  printf ("low %d %d ", low_cnt, high_cnt);
	  printf ("%dHz\n", 10000000/((low_cnt + high_cnt) * 204));
	  high_cnt = 0;
	  lcd_line (k, low, k, high);
	}
      r ? high_cnt++ : low_cnt++;
      lcd_dot (k, r ? high : low, 1);
      or = r;
    }
}
