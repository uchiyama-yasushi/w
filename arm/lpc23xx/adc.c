
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

// 10bit Analog-to-Digital Converter. 0-3V

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>

STATIC struct adc_pin
{
  int port;
  int bit;
  int function;
} __adc_pin [] = {
  // AD0.0-AD0.3 : 0.23-0.26 func 1
  // AD0.4-5 : 1.30-1.31 func 3
  // AD0.6-7 :0.12-0.13 func 3
  { 0, 23, 1 },
  { 0, 24, 1 },
  { 0, 25, 1 },
  { 0, 26, 1 },
  { 1, 30, 3 },
  { 1, 31, 3 },
  { 0, 12, 3 },
  { 0, 13, 3 },
};

void
adc_init ()
{
  // Power on (On reset, the ADC is disabled.)
  mcu_peripheral_power (PCONP_PCAD, TRUE);
  mcu_peripheral_clock (PCLK_ADC, CCLK8);	//72/8=9MHz

  // Enable ADC
  *ADC_CR = CR_PDN | ((2 & CR_CLKDIV_MASK) << CR_CLKDIV_SHIFT); // 4.5MHz
}

void
adc_fini ()
{

  // Disable Interrupt.
  *ADC_INTEN = 0;
  // Power-down mode.
  *ADC_CR = 0;
  // Power off.
  mcu_peripheral_power (PCONP_PCAD, FALSE);
}

void
adc_channel_enable (int ch)
{
  struct adc_pin *pin = __adc_pin + ch;
  uint32_t r;

  // Connect target pin to ADC
  mcu_pin_select (pin->port, pin->bit, pin->function);
  mcu_pin_mode (pin->port, pin->bit, PIN_HIZ);

  // Enable channel.
  r = *ADC_CR & CR_MASK;
  *ADC_CR = r | (1 << ch);
}

void
adc_channel_disable (int ch)
{
  struct adc_pin *pin = __adc_pin + ch;

  // Disable channel.
  uint32_t r = *ADC_CR & CR_MASK;
  r &= ~(1 << ch);
  *ADC_CR = r;

  mcu_pin_mode (pin->port, pin->bit, PIN_PULLUP);
}

bool
adc_conversion (int ch, uint32_t *val)
{
  uint32_t r = *ADC_CR & CR_MASK;
  volatile uint32_t *dr = (volatile uint32_t *)(ADC_DR_BASE + ch * 4);

  // Start conversion.
  *ADC_CR = r | CR_START_NOW;

  while (((r = *dr) & (DR_OVERRUN | DR_DONE)) == 0)
    ;//XXX timeout

  if (r & DR_OVERRUN)
    {
      printf ("%s: overrun\n", __FUNCTION__);
      return FALSE;
    }

  *val = (r >> DR_V_SHIFT) & DR_V_MASK;

  return TRUE;
}
