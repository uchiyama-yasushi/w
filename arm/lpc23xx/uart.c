
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

// Universal Asynchronous Receiver/Transmitter. 0,2,3
// UART1 has modem interface.
// On reset UART0 is enabled. UART2,3 are disabled.

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>

STATIC struct uart
{
  addr_t base;
  int clock;
  int power;
  int tx_port;
  int tx_bit;
  int rx_port;
  int rx_bit;
  int function;
  int vic_irq;
} __uart [] = {
  { UART0_BASE, PCLK_UART0, PCONP_PCUART0, 0,  2, 0,  3, 1, VIC_UART0 },
  { UART1_BASE, PCLK_UART1, PCONP_PCUART1, 0, 15, 0, 16, 1, VIC_UART1 },
  { UART2_BASE, PCLK_UART2, PCONP_PCUART2, 0, 10, 0, 11, 1, VIC_UART2 },
#if 0	// There are 2 configurations.
  { UART3_BASE, PCLK_UART3, PCONP_PCUART3, 0, 25, 0, 26, 3, VIC_UART3 },
#else
  { UART3_BASE, PCLK_UART3, PCONP_PCUART3, 4, 28, 4, 29, 3, VIC_UART3 },
#endif
};

void
uart_init (enum uart_channel channel, struct uart_clock_conf *conf, bool
	   interrupt)
{
  struct uart *u = &__uart[channel];
  addr_t base = u->base;

  // Pin select
  mcu_pin_select (u->tx_port, u->tx_bit, u->function); // Port0.2 : TXD0
  mcu_pin_mode (u->tx_port, u->tx_bit, PIN_PULLUP);
  // UART receive pins should not have pull-down registors enabled.
  mcu_pin_select (u->rx_port, u->rx_bit, u->function); // Port0.3 : RXD0
  mcu_pin_mode (u->rx_port, u->rx_bit, PIN_PULLUP);

  // Clock supply
  mcu_peripheral_clock (u->clock, conf->cclk_divider);

  // Power supply
  mcu_peripheral_power (u->power, TRUE);

  // Set speed.
  *ULCR (base) = ULCR_DLAB;	// Enable Divisor Latche access.
  *UDLM (base) = conf->DLM;
  *UDLL (base) = conf->DLL;
  *UFDR (base) = conf->FDR;

  // Set line format. 8N1.
  *ULCR (base) = ULCR_8BIT | ULCR_PARITY_NONE | ULCR_STOP1 | ULCR_BREAK_DISABLE;

  if (!interrupt)
    {
      *UIER (base) = 0;
    }
  else
    {
      *VICIntSelect |= u->vic_irq; // FIQ
      *VICIntEnable |= u->vic_irq;
      *UIER (base) = 1;
    }
}

void
uart_putc (enum uart_channel channel, int8_t c)
{
  addr_t base = __uart[channel].base;

  while ((*ULSR (base) & 0x20) == 0)
    ;
  *UTHR (base) = c;
}

int8_t
uart_getc (enum uart_channel channel)
{
  addr_t base = __uart[channel].base;
  uint8_t c;

  while ((*ULSR (base) & 0x01) == 0)
    ;
  c = *URBR (base);

  return c;
}
