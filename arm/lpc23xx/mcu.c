
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
#include <reg.h>

uint32_t mcu_sysclock;
uint32_t mcu_cpuclock;

STATIC uint32_t mcu_state[4];
void
__mcu_mode_save ()
{

  mcu_state[0] = *CLKSRCSEL;
  mcu_state[1] = *PLLSTAT;
  mcu_state[2] = *PLLCFG;
  mcu_state[3] = *CCLKCFG;
}

void
__mcu_mode_print ()
{
  const char *clksrc[] = { "IRC", "MAIN", "RTC" };

  iprintf ("Clock: %s", clksrc[mcu_state[0]]);
  iprintf (", PLL %sconnected", mcu_state[1] & PLLSTAT_PLLC ? "" : "NOT ");
  iprintf (", PLL MSEL:%d NSEL:%d", mcu_state[2] & PLLCFG_MSEL_MASK,
	   (mcu_state[2] >> PLLCFG_NSEL_SHIFT) & PLLCFG_NSEL_MASK);
  iprintf (", CCLKCFG %d\n", mcu_state[3]);
}

void
mcu_mode ()
{

  __mcu_mode_save ();
  __mcu_mode_print ();
  iprintf ("sysclock:%dHz cpuclock:%dHz\n", mcu_sysclock, mcu_cpuclock);
  iprintf ("PCLK0: %x, PCLK1: %x PCONP: %x\n", *PCLKSEL0, *PCLKSEL1, *PCONP);
}

void
cpu_sleep ()
{
  // XXX notyet I want to rename mcu_sleep.
}

void
cpu_reset ()	//I want to rename mcu_sleep.
{
  // Change vector table to ROM.
  *MEMMAP = MAP_USERFLASH;

  // Disable IRQ, FIQ
  intr_disable ();

  // Disable VIC.
  *VICIntEnable = 0;
  // Clear pending interrupt.
  *VICIntEnClr = ~0;

  // Execute Reset vector.
  __asm volatile ("mov pc, %0;nop;nop" :: "r" (0));
  while (/*CONSTCOND*/1)
    ;
  // NOTREACHED
}

static inline void
__pll_feed ()
{

  *PLLFEED = 0xaa;
  *PLLFEED = 0x55;
}

void
mcu_init ()
{
  // Flash memory setting.
  *MAMCR = MAM_DISABLE;
}

void
mcu_speed (struct cpu_clock_conf *conf)
{
  uint32_t r;

  mcu_init ();// Flash memory setting.

  // User manual 6.14 PLL setup sequence.

  // 1. Disconnect the PLL with one feed sequence if PLL is already connected.
  if (*PLLSTAT & PLLSTAT_PLLC)	// PLL connected?
    {
      // Disconnect PLL. (Enable)
      *PLLCON = PLLCON_PLLE;
      __pll_feed ();
    }

  // 2. Disbale the PLL with one feed sequence.
  *PLLCON = 0;
  __pll_feed ();

  // 3. Change the CPU Clock Divider setting to speed up operation
  // without the PLL, If desired

  // 4. Write to the Clock Source Selection Control register to change
  // clock source.
  // Set main oscillator as PLL clock source.

  // Enable main oscillator.
  r = *SCS;
  r |= SCS_OSCEN;
  if (conf->oscillator_freq > 15000000)
    r |= SCS_OSCRANGE;
  else
    r &= ~SCS_OSCRANGE;
  *SCS = r;
  // Wait until oscillator ready.
  while (!(*SCS & SCS_OSCSTAT))
    ;
  *CLKSRCSEL = CLKSRCSEL_OSC;

  // 5. Write to the PLLCFG and make it effective with one feed
  // sequence. The PLLCFG can only be updated when the PLL is disabled.
  *PLLCFG = (conf->m - 1) | ((conf->n - 1) << PLLCFG_NSEL_SHIFT);
  // Fcco(PLLclock) = 2 * (MVAL+1) * Fin(12MHz) / (NVAL + 1)
  // 2 * 12 * 12000000 / 1 = 288MHz
  mcu_sysclock = (2 * conf->m * conf->oscillator_freq) / conf->n;
  __pll_feed ();

  // 6. Enable the PLL with one feed sequence.
  // Enable PLL, Disconnected.
  *PLLCON = PLLCON_PLLE;
  __pll_feed ();

  // 7. Change the CPU Clock Divider setting for the operation with
  // the PLL. It's critical to do this before connecting PLL.
  // CPU clock divider.
  // CPUclock = PLLclock / (CCLKCFG + 1)
  // 288/4 = 72MHz
  mcu_cpuclock = mcu_sysclock / conf->cpudiv;
  *CCLKCFG = conf->cpudiv - 1;

  // 8. Wait for PLL to achive lock.
  while ((*PLLSTAT & PLLSTAT_PLOCK) == 0)
    ;
  // Now PLL is requested frequency.

  // 9. Connect PLL with one feed sequence.
  *PLLCON = PLLCON_PLLE | PLLCON_PLLC;		/* enable and connect */
  __pll_feed ();
  while ((*PLLSTAT & PLLSTAT_PLLC) == 0)
    ;
  // Now PLL is connected.
}

enum pclk_divide
mcu_peripheral_clock (enum pclk_select device, enum pclk_divide div)
{
  volatile uint32_t *r = device & 0x80 ? PCLKSEL1 : PCLKSEL0;
  uint32_t r0 = *r;
  int shift = device & 0x7f;

  *r = (r0 & ~(0x3 << shift)) | (div << shift);

  return (enum pclk_divide)((r0 >> shift) & 0x3);// return previous setting.
}

bool
mcu_peripheral_power (int bit, bool on)
{
  uint32_t r = *PCONP;

  if (on)
    *PCONP = r | bit;
  else
    *PCONP = r & ~bit;

  return r & bit; // return previous status.
}

void
mcu_pin_select (int port, int pin, int func)
{
  volatile uint32_t *r = PINSEL0 + port * 2 + ((pin > 15) ? 1 : 0);
  int shift = (pin & 0xf) << 1;

  *r = (*r & ~(0x3 << shift)) | func << shift;
}

void
mcu_pin_select2 (int port, int pin, enum pinsel func)
{
  volatile uint32_t *r = PINSEL0 + port * 2 + ((pin > 15) ? 1 : 0);
  int shift = (pin & 0xf) << 1;

  // P0_31 ... don't exist.
  assert (!(port == 0 && pin == 31));
  iprintf ("port %d pin %d func %d => r=%x shift=%d\n", port, pin,
	   func, r, shift);

  *r = (*r & ~(0x3 << shift)) | func << shift;
}

void
mcu_pin_mode (int port, int pin, enum pinmode mode)
{
  volatile uint32_t *r = PINMODE0 + port * 2 + ((pin > 15) ? 1 : 0);
  int shift = (pin & 0xf) << 1;

  // P0_[27-30] can't use internal pullup.
  assert (!((port == 0) && ((pin >=27) && (pin <=31))));
  iprintf ("port %d pin %d func %d => r=%x shift=%d\n", port, pin,
	   mode, r, shift);

  *r = (*r & ~(0x3 << shift)) | mode << shift;
}

void
mcu_fast_gpio (bool fast)
{
  if (fast)
    *SCS |= SCS_GPIOM;	// Fast GPIO mode.
  else
    *SCS &= ~SCS_GPIOM;	// Legacy GPIO mode.
}
