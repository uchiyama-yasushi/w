
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

// Kionix KXP84 tri-axis accelerometer.
/*
  Output range: +-2g (19.6m/s^2)
  A/D Conversion time: 200us
  SPI Communication Rate: 1MHz
  I2C Communication Rate: 400KHz
  Zero-g Offset: 1946(min)-2048(target)-2150(max) counts. (+-102)
  Sensitivity(counts/g): 794-819-844 (+-25)
 */


#ifndef _DEVICE_KXP84_H_
#define	_DEVICE_KXP84_H_
// I2C Slave addresses. LSB 1bit is ADDR0 of KXP84
#define	KXP84_I2C_ADDR0	0x18
#define	KXP84_I2C_ADDR1	0x19

// KXP84 Embedded Registers. (I2C mode)
#define	KXP84_REG_XOUT_H	0x00	//(R)
#define	KXP84_REG_XOUT_L	0x01	//(R)
#define	KXP84_REG_YOUT_H	0x02	//(R)
#define	KXP84_REG_YOUT_L	0x03	//(R)
#define	KXP84_REG_ZOUT_H	0x04	//(R)
#define	KXP84_REG_ZOUT_L	0x05	//(R)
// Free-fall interrupt threashold.
/* Threashold(g) = 16 * FF_INT(counts) * Sensitivity(g/count)
   Sensitivity = 1/819
   FF_INT = g / 16 * 819

   FF_INT  Threashold(g)
   0x0f	   0.3
   0x14    0.4
   0x19    0.5
   MOT_INT Threashold
   0x4c	   1.5
   0x51	   1.6
*/
#define	KXP84_REG_FF_INT	0x06
// Free-fall delay/debounce time
/*
  Delay(sec) = FF_DELAY(counts) / Interrupt Sampling Rate(Hz)
  At Interrupt Sampling Rate: 250Hz,
  FF_DELAY = Delay(sec) * 250

  0x14 = 80msec * 250
 */
#define	KXP84_REG_FF_DELAY	0x07
// Motion activated interrupt acceleration threshold.
#define	KXP84_REG_MOT_INT	0x08
// Motion activated delay/bounce time.
#define	KXP84_REG_MOT_DELAY	0x09

#define	KXP84_REG_REGC		0x0a
// Select the rate at which the accelerometer is sampled when
// debouncing a potential interrupt event.
#define	 REGC_IntSpd0	0x01
#define	 REGC_IntSpd1	0x02
#define	  REGC_FREQ_250Hz	0
#define	  REGC_FREQ_1KHz	REGC_IntSpd0
#define	  REGC_FREQ_4KHz	REGC_IntSpd1
#define	  REGC_FREQ_16KHz	(REGC_IntSpd0 | REGC_IntSpd1)
#define	 REGC_MOTLatch	0x08
#define	 REGC_FFLatch	0x10

#define	KXP84_REG_REGB		0x0b
#define	 REGB_FFMOTI	0x01	// MOTI || FFI -> pin 4
#define	 REGB_FFIen	0x02	// pin 4
#define	 REGB_MOTIen	0x04	// pin 3
#define	 REGB_ST	0x20	// Self test
#define	 REGB_nENABLE	0x40	// low-power standby.
#define	 REGB_CLKhld	0x80	// SCL held low during A/D conversions.

#define	KXP84_REG_REGA		0x0c	//(R)
#define	 REGA_FFI	0x01
#define	 REGA_MOTI	0x02
#define	 REGA_Parity	0x04	// EEPROM parity. should be 0.


#define	KXP84_ACCEL_DATA(h, l)	(((h) << 4) & 0xf00) | (((l) >> 4) & 0xf)

#define	KXP84_ADC_TIME	200	// usec for A/D conversion.

#endif
