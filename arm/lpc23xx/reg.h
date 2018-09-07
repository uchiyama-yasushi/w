
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
// LPC2388

#ifndef _REG_H_
#define	_REG_H_

#include "mam.h"
#include "scb.h"
#include "clkpwr.h"
#include "pinsel.h"
#include "uart.h"
#include "gpio.h"
#include "vic.h"
#include "timer.h"
#include "mci.h"
#include "dma.h"
#include "rtc.h"
#include "pwm.h"
#include "adc.h"
#include "dac.h"
#include "spi.h"
#include "i2c.h"

// MCU dependent.
__BEGIN_DECLS
struct cpu_clock_conf;

void mcu_fast_gpio (bool);

void __mcu_mode_save (void);
void __mcu_mode_print (void);
void mcu_mode (void);
void mcu_speed (struct cpu_clock_conf *);
bool mcu_peripheral_power (int, bool);
enum pclk_divide mcu_peripheral_clock (enum pclk_select, enum pclk_divide);
void cpu_sleep (void);
void cpu_reset (void) __attribute__((noreturn));

extern uint32_t mcu_sysclock;
extern uint32_t mcu_cpuclock;

struct cpu_clock_conf
{
  int	m;
  int	n;
  int	cpudiv;
  int	oscillator_freq;
};
extern struct cpu_clock_conf cpu_clock_conf;
extern struct uart_clock_conf uart_clock_conf;

__END_DECLS
#endif
