
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

// Serial Peripheral Interface
//#define	SPI_VERIFY
#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>

bool spi_success (void);

void
spi_init ()
{

  mcu_peripheral_power (PCONP_PCSPI, TRUE);
  mcu_peripheral_clock (PCLK_SPI, CCLK8);	//72/8=9MHz
  mcu_pin_select (0, 15, 3); //SCK
  //  mcu_pin_select (0, 16, 3); //SSEL
  //  mcu_pin_select (0, 17, 3); //MISO
  mcu_pin_select (0, 16, 0); //SSEL
  mcu_pin_select (0, 17, 0); //MISO

  mcu_pin_select (0, 18, 3); //MOSI
  *S0SPCCR = 8;	// 9/8 = 1.125MHz
  //  *S0SPCCR = 0xff;	// 9/8 = 1.125MHz
  *S0SPCR = CR_MSTR;	// Master. 8bit MSB
  //  *S0SPCR = CR_MSTR | CR_CPOL |CR_CPHA;	// Master. 8bit MSB
  //  *S0SPCR = CR_MSTR | CR_LSBF;	// Master. 8bit LSB
}

void
spi_fini ()
{

  mcu_peripheral_power (PCONP_PCSPI, FALSE);
}

void
spi_putc(uint8_t c)
{
  //  uint32_t r;

  *S0SPDR = c;
  while (!(*S0SPSR & SR_SPIF))
    ;
  //  r = *S0SPDR;

  //  spi_success ();

}

bool
spi_success ()
{
  uint32_t r = *S0SPSR;
#ifdef SPI_VERIFY
  iprintf ("%x %x\n", *SPTCR, *SPTSR);
#endif

  if (!(r & SR_ERRBITS))
    return TRUE;

  if (r & SR_ABRT)
    iprintf ("Slave abort ");
  if (r & SR_MODF)
    iprintf ("Mode fault ");
  if (r & SR_ROVR)
    iprintf ("Read overrun ");
  if (r & SR_WCOL)
    iprintf ("Write collision ");

  iprintf ("\n");
  return FALSE;
}
