
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

#ifndef _LPC23XX_CLKPWR_H_
#define	_LPC23XX_CLKPWR_H_
// Clocking and Power Control

/* Clock Source Selection */
// Clock Source Select Register. determine PLL clock source.
#define	CLKSRCSEL	((volatile uint32_t *)0xe01fc10c)
#define	 CLKSRCSEL_IRC	0x00000000	// Internal RC oscillator.
#define	 CLKSRCSEL_OSC	0x00000001	// Main oscillator.
#define	 CLKSRCSEL_RTC	0x00000002	// RTC

/* Phase Locked Loop */
// PLL Control Register
#define	PLLCON		((volatile uint32_t *)0xe01fc080)
#define	 PLLCON_PLLE	0x00000001	// Enable
#define	 PLLCON_PLLC	0x00000002	// Connect

// PLL Configuration Register
#define	PLLCFG		((volatile uint32_t *)0xe01fc084)
// MSEL = M-1 M:6-512
#define	 PLLCFG_MSEL_MASK	0x00007fff // Multiplier
#define	 PLLCFG_MSEL_SHIFT	0
// NSEL = N-1 N:1-32
#define	 PLLCFG_NSEL_MASK	0x000000ff // Pre-Divider
#define	 PLLCFG_NSEL_SHIFT	16

// PLL Status Register
#define	PLLSTAT		((volatile uint32_t *)0xe01fc088)
#define	 PLLSTAT_MSEL_MASK	0x00007fff // Multiplier
#define	 PLLSTAT_NSEL_MASK	0x00ff0000 // Pre-Divider
#define	 PLLSTAT_PLLE		0x01000000 // Enable
#define	 PLLSTAT_PLLC		0x02000000 // Connect
#define	 PLLSTAT_PLOCK		0x04000000 // Lock


// PLL Feed Register
#define	PLLFEED		((volatile uint32_t *)0xe01fc08c)

/* Clock Dividers. */
// CPU Clock Configuration Register
#define	CCLKCFG		((volatile uint32_t *)0xe01fc104)
// divide value must be odd. CCLKSEL = divider value - 1. 0-255

// USB Clock Configuration Register
#define	USBCLKCFG	((volatile uint32_t *)0xe01fc108)
// IRC Trim Register
#define	IRCTRIM		((volatile uint32_t *)0xe01fc1a4)
// Peripheral Clock Selection Register 0
#define	PCLKSEL0	((volatile uint32_t *)0xe01fc1a8)
// Peripheral Clock Selection Register 1
#define	PCLKSEL1	((volatile uint32_t *)0xe01fc1ac)
enum pclk_select
  {
    PCLK_WDT	= 0,
    PCLK_TIMER0	= 2,
    PCLK_TIMER1	= 4,
    PCLK_UART0	= 6,
    PCLK_UART1	= 8,
    PCLK_PWM1	= 12,
    PCLK_I2C0	= 14,
    PCLK_SPI	= 16,
    PCLK_RTC	= 18,
    PCLK_SSP1	= 20,
    PCLK_DAC	= 22,
    PCLK_ADC	= 24,
    PCLK_CAN1	= 26,
    PCLK_CAN2	= 28,
    PCLK_ACF	= 30,
    PCLK_BAT_RAM= 0x80 | 0,
    PCLK_GPIO	= 0x80 | 2,
    PCLK_PCB	= 0x80 | 4,
    PCLK_I2C1	= 0x80 | 6,
    PCLK_SSP0	= 0x80 | 10,
    PCLK_TIMER2	= 0x80 | 12,
    PCLK_TIMER3	= 0x80 | 14,
    PCLK_UART2	= 0x80 | 16,
    PCLK_UART3	= 0x80 | 18,
    PCLK_I2C2	= 0x80 | 20,
    PCLK_I2S	= 0x80 | 22,
    PCLK_MCI	= 0x80 | 24,
    PCLK_SYSCON	= 0x80 | 28,
  };

enum pclk_divide
  {
    CCLK4 = 0,// PCLK = CCLK/4
    CCLK1 = 1,// PCLK_RTC only this is illegal
    CCLK2 = 2,
    CCLK8 = 3,// For CAN1, CAN2, this is CCLK/6
  };

/* Power Control */
// Power Control Register
#define	PCON		((volatile uint32_t *)0xe01fc0c0)
// Interrupt Wakeup Register
#define	INTWAKE		((volatile uint32_t *)0xe01fc144)
// Power Control for Peripherals Register.
#define	PCONP		((volatile uint32_t *)0xe01fc0c4)
#define	 PCONP_PCTIM0	(1 << 1)
#define	 PCONP_PCTIM1	(1 << 2)
#define	 PCONP_PCUART0	(1 << 3)
#define	 PCONP_PCUART1	(1 << 4)
#define	 PCONP_PCPWM1	(1 << 6)
#define	 PCONP_PCI2C0	(1 << 7)
#define	 PCONP_PCSPI	(1 << 8)
#define	 PCONP_PCRTC	(1 << 9)
#define	 PCONP_PCSSP1	(1 << 10)
#define	 PCONP_PCEMC	(1 << 11)
#define	 PCONP_PCAD	(1 << 12)
#define	 PCONP_PCAN1	(1 << 13)
#define	 PCONP_PCAN2	(1 << 14)
#define	 PCONP_PCI2C1	(1 << 19)
#define	 PCONP_PCSSP0	(1 << 21)
#define	 PCONP_PCTIM2	(1 << 22)
#define	 PCONP_PCTIM3	(1 << 23)
#define	 PCONP_PCUART2	(1 << 24)
#define	 PCONP_PCUART3	(1 << 25)
#define	 PCONP_PCI2C2	(1 << 26)
#define	 PCONP_PCI2S	(1 << 27)
#define	 PCONP_PCSDC	(1 << 28)
#define	 PCONP_PCGPDMA	(1 << 29)
#define	 PCONP_PCENET	(1 << 30)
#define	 PCONP_PCUSB	(1 << 31)

#endif
