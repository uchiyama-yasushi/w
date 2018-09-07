
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
// H8/3664

#ifndef _REG_H_
#define	_REG_H_
// MCU mode.
// Normal mode (64K address space) only.

// Low Power
// System Control Register 1, 2
#define	SYSCR1			((volatile uint8_t *)0xfff0)
#define	SYSCR2			((volatile uint8_t *)0xfff1)
// Module STanby Control Register
#define	MSTCR1			((volatile uint8_t *)0xfff9)

// EEPROM
#define	EEPROM_EKR		((volatile uint8_t *)0xff10)

// ROM
// FLash Memory Control Register 1
#define	ROM_FLMCR1		((volatile uint8_t *)0xff90)
// FLash Memory Control Register 2
#define	ROM_FLMCR2		((volatile uint8_t *)0xff91)
// FLash Memory PoWer Control Register
#define	ROM_FLPWCR		((volatile uint8_t *)0xff92)
// Block select Register 1
#define	ROM_EBR1		((volatile uint8_t *)0xff93)
// Flash memory ENable Register
#define	ROM_FENR		((volatile uint8_t *)0xff9b)

#include <intc.h>
#include <sci3.h>
#include <ioport.h>
#include <timer_a.h>
#include <timer_v.h>
#include <timer_w.h>
#include <addrbrk.h>
#include <adc.h>
#include <wdt.h>
#include <i2c.h>

// MCU dependend.
__BEGIN_DECLS
void mcu_mode (void);
__END_DECLS

#endif

