
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

// Using Akizuki AEKXP84-2050 Accelerometer

#include <sys/system.h>
#include <sys/console.h>
#include <sys/delay.h>

#include <cpu.h>
#include <reg.h>
#include <device/kxp84_i2c.h>
#include "accelerometer.h"
#include "local.h"

STATIC void kxp84_reset (void);
uint32_t pack_accel (uint16_t, uint16_t, uint16_t);
void unpack_accel (uint32_t, uint16_t *, uint16_t *, uint16_t *);

STATIC uint8_t __kxp84_buf[8]; // KXP84 output buffer.

bool
accelerometer_init ()
{
  // kxp routines are interrupt drive. interrupt must be enabled.
  assert ((intr_status () & (PSR_I | PSR_F)) == 0);
  // KXP84 reset pin connected to P1_19
  mcu_pin_select (1, 19, 0);	// GPIO
  mcu_pin_mode (1, 19, PIN_PULLUP);
  gpio_dir (1, 19, GPIO_OUTPUT);

  // Initialize accelerometer module.
  if (!kxp84_init (kxp84_reset))
    {
      DPRINTF ("*** kxp84 initialize failed.\n");
      return FALSE;
    }

  kxp84_read_setup (__kxp84_buf);

  return TRUE;
}

bool
accelerometer_read (uint32_t *v)
{
  uint16_t x, y, z;

  if (!kxp84_read (&x, &y, &z))
    return FALSE;
  // for each axis 12bit data converted to 10bit.

  *v = pack_accel (x, y, z);
#if 0
  unpack_accel (*v, &x, &y, &z);
  iprintf ("%d %d %d\n", x, y, z);
#endif
  return TRUE;
}

void
kxp84_reset ()
{

  GPIO_PIN_SET (1, 19);
  udelay (100);
  GPIO_PIN_CLR (1, 19);
  udelay (100);
}

uint32_t
pack_accel (uint16_t x, uint16_t y, uint16_t z)
{
  uint32_t a =
    (((x >> 2) & 0x3ff) << 20) |
    (((y >> 2) & 0x3ff) << 10) |
    ((z >> 2) & 0x3ff);

  return a;
}

void
unpack_accel (uint32_t a, uint16_t *x, uint16_t *y, uint16_t *z)
{
  *x = (a >> 20) & 0x3ff;
  *y = (a >> 10) & 0x3ff;
  *z = a & 0x3ff;
}

// For debug
void
postprocess_accelerometer (uint32_t event __attribute__((unused)),
			   int32_t *buf __attribute__((unused)))
{
#if 0//def DEBUG
  uint16_t x, y, z;

  unpack_accel (*buf, &x, &y, &z);
  iprintf ("%d %d %d\n", x, y, z);
#endif
}

