
/*-
 * Copyright (c) 2010 UCHIYAMA Yasushi.  All rights reserved.
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

// SH2A 7262 Power unit
#ifndef _SH2A_7262_PWR_H_
#define	_SH2A_7262_PWR_H_

/* Standby Control Register */
// Standby setting
#define	PWR_STBCR1		((volatile uint8_t *)0xfffe0014)
#define	 STBCR1_STBY		(1 << 7)
#define	 STBCR1_DEEP		(1 << 6)
// Module power
#define	PWR_STBCR2_ADDR		0xfffe0018
#define	PWR_STBCR3_ADDR		0xfffe0408
#define	PWR_STBCR4_ADDR		0xfffe040c
#define	PWR_STBCR5_ADDR		0xfffe0410
#define	PWR_STBCR6_ADDR		0xfffe0414
#define	PWR_STBCR7_ADDR		0xfffe0418
#define	PWR_STBCR8_ADDR		0xfffe040c

/* Software Reset Control Register */
#define	PWR_SWRSTCR		((volatile uint8_t *)0xfffe0430)
/* System Control Register */
#define	PWR_SYSCR1		((volatile uint8_t *)0xfffe0400)
#define	PWR_SYSCR2		((volatile uint8_t *)0xfffe0404)
#define	PWR_SYSCR3		((volatile uint8_t *)0xfffe0420)
#define	PWR_SYSCR4		((volatile uint8_t *)0xfffe0424)

/* */
#define	PWR_RRAMKP		((volatile uint8_t *)0xfffe6800)
/* Deep Stanby Control Register */
#define	PWR_DSCTR		((volatile uint8_t *)0xfffe6802)
/* Deep Stanby Select Register */
#define	PWR_DSSSR		((volatile uint16_t *)0xfffe6804)
/* Deep Stanby Edge Select Register */
#define	PWR_DSESR		((volatile uint16_t *)0xfffe6806)
/* Deep Stanby Flag Register */
#define	PWR_DSFR		((volatile uint16_t *)0xfffe6808)
/* XTAL Gain Control Register */
#define	PWR_XTALCTR		((volatile uint8_t *)0xfffe6810)

__BEGIN_DECLS

enum module_power
{
#define	_(a, b)	(PWR_STBCR ## a ## _ADDR << 4) | b
  PWR_UDI	= _(2, 7),
  PWR_DMA	= _(2, 5),
  PWR_FPU	= _(2, 4),
  PWR_PORT_HIZ	= _(3, 7),
  PWR_IEBUS	= _(3, 6),
  PWR_TPU	= _(3, 5),
  PWR_SDCARD0	= _(3, 4),
  PWR_SDCARD1	= _(3, 3),
  PWR_ADC	= _(3, 2),
  PWR_RTC	= _(3, 0),
  PWR_SCIF0	= _(4, 7),
  PWR_SCIF1	= _(4, 6),
  PWR_SCIF2	= _(4, 5),
  PWR_SCIF3	= _(4, 4),
  PWR_SCIF4	= _(4, 3),
  PWR_SCIF5	= _(4, 2),
  PWR_SCIF6	= _(4, 1),
  PWR_SCIF7	= _(4, 0),
  PWR_I2C0	= _(5, 7),
  PWR_I2C1	= _(5, 6),
  PWR_I2C2	= _(5, 5),
  PWR_CAN0	= _(5, 3),
  PWR_CAN1	= _(5, 2),
  PWR_SPI0	= _(5, 1),
  PWR_SPI1	= _(5, 0),
  PWR_SOUND0	= _(6, 7),
  PWR_SOUND1	= _(6, 6),
  PWR_SOUND2	= _(6, 5),
  PWR_SOUND3	= _(6, 4),
  PWR_CDROM	= _(6, 3),
  PWR_SAMPCONV0	= _(6, 2),
  PWR_SAMPCONV1	= _(6, 1),
  PWR_USB	= _(6, 0),
  PWR_CLKSIO	= _(7, 7),
  PWR_SPDIF	= _(7, 6),
  PWR_VIDEO	= _(7, 4),
  PWR_CMT	= _(7, 2),
  PWR_NAND	= _(7, 0),
  PWR_PWM	= _(8, 7),
  PWR_DECOMP	= _(8, 0),
#undef _
};

void pwr_init (void);
void pwr_peripheral_module (enum module_power, bool);
void pwr_info (void);
__END_DECLS
#endif
