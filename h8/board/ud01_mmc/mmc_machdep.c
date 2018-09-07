
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
#include <sys/mmc.h>
#include <sys/delay.h>

STATIC uint8_t bit_inverse (uint8_t);

void
md_mmc_init ()
{
  *P6_DDR |= 0x2;

  *P9_DDR = 0xf3;	// RXD0, RXD1 input, else output.
  *P9_DR = 0;

  *SCI0_SCR = 0;	// Internal clock.
  // Don't reorder SCR->SMR sequence.
#if 0
  *SCI0_SMR = SMR_CM | SMR_CKS0 | SMR_CKS1;	// 1/64 clock
  //  *SCI0_BRR = 155;
  *SCI0_BRR = 100;
#else
  *SCI0_SMR = SMR_CM;	// 1/1 clock
  *SCI0_BRR = 155;
#endif
}

void
md_mmc_activate ()
{

  md_mmc_putc (0xff);
  *P6_DR &= ~0x2;	// CS# Assert;
  udelay (1);
}

void
md_mmc_deactivate ()
{

  *P6_DR |= 0x2;	// CS# Deassert.
}

int8_t
md_mmc_getc ()
{
  uint8_t c;
  uint8_t r;

  *SCI0_SCR |= (SCR_TE | SCR_RE);

  do
    {
      r = *SCI0_SSR;
      if (r & SSR_RDRF)
	{
	  iprintf ("drain receive buffer.\n");
	  *SCI0_SSR &= ~SSR_RDRF;
	}
    }
  while ((r & SSR_TDRE) == 0);

  *SCI0_TDR = 0xff;
  *SCI0_SSR &= ~SSR_TDRE;

  while (((c = *SCI0_SSR) & (SSR_RDRF | SSR_ERR_BITS)) == 0)
    ;
  if (c & SSR_ERR_BITS)
    {
      *SCI0_SSR &= ~(SSR_RDRF | SSR_ERR_BITS);
      iprintf ("error\n");
      return 0;
    }
  c = *SCI0_RDR;
  *SCI0_SSR &= ~SSR_RDRF;
  *SCI0_SCR &= ~(SCR_TE | SCR_RE);

  return bit_inverse (c);
}


void
md_mmc_putc (int8_t c)
{
  uint8_t r = bit_inverse (c);

  *SCI0_SCR |= SCR_TE;
  while ((*SCI0_SSR & SSR_TDRE) == 0)
    ;
  *SCI0_TDR = r;
  *SCI0_SSR &= ~SSR_TDRE;
  while ((*SCI0_SSR & SSR_TEND) == 0)
    ;
  // transmit done.
  *SCI0_SCR &= ~(SCR_TE);
}

uint8_t bit_inverse (uint8_t s)
{
  uint8_t d = 0;
  uint8_t bit = 0x80;
  int i;

  for (i = 0; i < 8; i++, bit >>= 1)
    d |= (((s & bit) ? 1 : 0) << i);

  return d;
}

