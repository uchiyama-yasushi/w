
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
#include <reg.h>
#include <sys/delay.h>
#include <string.h>

STATIC void kxp84_init (void);
STATIC void kxp84_reg_write (uint8_t, uint8_t, uint8_t);
STATIC void kxp84_reg_read (uint8_t, uint8_t, uint8_t *, int);
STATIC void kxp84_main (void);

#define	KXP84_I2C_ADDR	0x18
#define	KXP84_INTR_ENABLE

void
board_main (uint32_t arg __attribute__((unused)))
{
  intr_enable ();

  kxp84_init ();
  i2c_init ();
  kxp84_main ();
}

void
kxp84_init ()
{
  // P55 I/O -> RESET
  // P54 WKP4 <- FF
  // P53 WKP3 <- MOT
  // P52 I/O -> ADDR0
  // P51 I/O -> CS#
  *PMR5 = (1 << 4) | (1 << 3);	// WKP input
  *PCR5 = (1 << 5) | (1 << 2);	// Output port.
  *PUCR5 = (1 << 5) | (1 << 2) | (1 << 1); // Enable pullup MOS

  // Interrupt setting.
#ifdef KXP84_INTR_ENABLE
  *IEGR2 = (1 << 4) | (1 << 3);	// WKP rising edge.
  *IENR1 |= IENR1_IENWP; // WKP interrupt enable.
#endif
  // CS# deassert. (active low) -> I2C mode
  *PDR5 = 1;
  if (KXP84_I2C_ADDR & 1)
    *PDR5 |= (1 << 2);

#if 1
  *PDR5 |= (1 << 5);
  udelay (100);
  *PDR5 &= ~(1 << 5);
  udelay (100);
#endif
}

void
kxp84_reg_write (uint8_t slave_addr, uint8_t reg_addr, uint8_t data)
{

  i2c_start ();
  i2c_write ((slave_addr << 1) | 0);	// SAD + W
  i2c_write (reg_addr);
  i2c_write (data);
  i2c_stop ();
}


void
kxp84_reg_read (uint8_t slave_addr, uint8_t reg_addr, uint8_t *buf, int n)
{

  i2c_start ();
  i2c_write ((slave_addr << 1) | 0);	// SAD + W
  i2c_write (reg_addr);
  i2c_repeated_start ();
  i2c_write ((slave_addr << 1) | 1);	// SAD + R
  i2c_read_n (buf, n);
  i2c_stop ();
}

void
kxp84_main ()
{

#ifdef KXP84_INTR_ENABLE
  kxp84_reg_write (KXP84_I2C_ADDR, 0x0b, 0x06);	// MOT FF interrupt enable.
#else
  kxp84_reg_write (KXP84_I2C_ADDR, 0x0b, 0x00);
#endif
  kxp84_reg_write (KXP84_I2C_ADDR, 0x0a, 0x00);
  kxp84_reg_write (KXP84_I2C_ADDR, 0x06, 0x14);
  kxp84_reg_write (KXP84_I2C_ADDR, 0x07, 0x14);
  kxp84_reg_write (KXP84_I2C_ADDR, 0x08, 0x4d);
  kxp84_reg_write (KXP84_I2C_ADDR, 0x09, 0x14);
#if 1
  uint8_t buf[6];
  int16_t x, y, z;
  while (/*CONSTCOND*/1)
    {
#if 1
      int i;
      for (i = 0; i < 6; i++)
	{
	  buf[i] = 0;
	  kxp84_reg_read (KXP84_I2C_ADDR, i, buf + i, 1);
	}
#else
      kxp84_reg_read (KXP84_I2C_ADDR, 0, buf, 6);
#endif
      x = (((buf[1] << 4) & 0xf00) | buf[0]) & 0xfff;
      y = (((buf[3] << 4) & 0xf00) | buf[2]) & 0xfff;
      z = (((buf[5] << 4) & 0xf00) | buf[4]) & 0xfff;
      iprintf ("%d %d %d\n", x, y, z);
      mdelay (300);
    }
#endif
}

void
i2c_intr ()
{
  iprintf ("i2c intr\n");
}

void
wkp_intr ()
{
  iprintf ("wkp! %x\n", *IWPR);	// inquire interrupt status.
  *IWPR = 0;	// clear interrupt.
}
