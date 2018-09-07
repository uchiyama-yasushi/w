
/*-
 * Copyright (c) 2008-2009 UCHIYAMA Yasushi.  All rights reserved.
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
#include <sys/thread.h>
#include <sys/delay.h>
#include <sys/shell.h>

#include <reg.h>

#include <sys/board.h>

#include <libud01/ud01dev.h>

//#define	SRAM_2STATE_ACCESS

#if 1	// C version of entry_ud01.S initialize.
void
board_bsc_init ()
{
#define	IO_DEFAULT	0xff	// All output.
  *P4_DDR = IO_DEFAULT;
  *P4_PCR = 0x0;
  *P4_DR = 0x0;

  *P5_PCR = 0;		// Don't use pull-up MOS.

  *P6_DR = 0;		//  Internal LED P6-1, P6-2

  *P8_DDR = IO_DEFAULT;
  *P8_DR = 0x0;

  *P9_DDR = ~0x30;	// 4,5 input.

  *PA_DDR = IO_DEFAULT;
  *PA_DR = 0x0;

  *PB_DDR = IO_DEFAULT;
  *PB_DR = 0x0;

  // MCU Mode 5

  // Bus width.
  *BSC_ABWCR = 0xff; // All 8bit. D15-D8. D0-D7 are I/O Port (4)
  // Access state.
#ifdef SRAM_2STATE_ACCESS
  *BSC_ASTCR = 0x40;// CS4,5(Ext.RAM) 2 state-access.CS6 LCD is 3 state access.
#else
  *BSC_ASTCR = 0x70;// CS4,5(Ext.RAM) and CS6 LCD are 3 state access.
#endif
  // WSC programmable wait mode. 3 wait state.
  *BSC_WCR = 3;
  // CS6(LCD) uses WSC. Ext.RAM don't use wait state.
  *BSC_WCER = 0x40;
  // Bus release disable. BACK#, BREQ# -> Port 6[1:2]
  *BSC_BRCR = 0;
  // Enable Chip Select output 4-6
  *BSC_CSCR = 0x70;

  // Set address line (A0-A17) output.
  *P1_DDR = 0xff;
  *P2_DDR = 0xff;
  *P5_DDR = 0x01;	//use A16 only. A17-A19 are I/O port. (3 Push buttons)

  // AS#, RD#, HWR#, LWR#, WAIT#  Don't use external bus.
  *P6_DDR = 0x6;	// BACK#, BREQ# are I/O port. (LED(P6_2), MMC CS#(P6_1))
  // !!! BSC_WCER is not 0xff. so don't set P6_DDR bit 0. !!!
}
#endif

void
board_device_init (uint32_t arg __attribute__((unused)))
{
#ifndef LCD_DISABLE
  lcd_init ();
#endif
#if 1 // 8LED unit connected to P4
  int i;
  // LED test
  *P4_DDR = 0xff;
  *P4_PCR = 0x0;

  *P4_DR = 0;
  *P6_DR = 4;
  mdelay (10);
  for (i = 0; i < 8; i++)
    {
      *P6_DR = i & 6;
      *P4_DR = 1 << i;
      mdelay (10);
    }
  *P4_DR = 0;
  *P6_DR = 0;
#endif
}

uint8_t
push_switch ()
{
  static uint8_t old_r;
  uint8_t r, e;

  r = *P5_DR & 0xe;
  e = (r ^ old_r) & ~r; // Falling edge
  old_r = r;

  return e;
}

void
led (int n __attribute__((unused)), bool on)
{

  if (on)
    *P6_DR |= 4;
  else
    *P6_DR &= ~4;
}
