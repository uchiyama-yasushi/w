
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


#include <sys/types.h>
#include <stdio.h>
#include "tmr.h"

#define	clock_peripheral()	24000000
uint8_t tmr_constant_calc (/*struct tmr_config *config, */uint32_t usec);

int
main (int argc, char *argv[])
{
  //  int i = atoi (argv[1]);
  uint32_t i, j;
  uint8_t cnt;
  for (i = 1; i < 0xffff; i++)
    {
      j = i;
      //      j = i *1000;
      if ((cnt = tmr_constant_calc (j)) == 0)
	{
	  printf ("no setting %d msec\n", i);
	}
    }

  return 0;
}

uint8_t
tmr_constant_calc (/*struct tmr_config *config, */uint32_t usec)
{
  static struct
  {
    int val;
    uint8_t CKS;
    uint8_t ICKS;
  } div[] = {
    { 2, TMR_CKS_INTCLK_2_8, TMR_ICKS_div2_RE },
    { 8, TMR_CKS_INTCLK_2_8, TMR_ICKS_div8_RE },
    { 32, TMR_CKS_INTCLK_32_64, TMR_ICKS_div32_RE },
    { 64, TMR_CKS_INTCLK_32_64, TMR_ICKS_div64_RE },
    { 1024, TMR_CKS_INTCLK_1024_8192, TMR_ICKS_div1024_RE },
    { 8192, TMR_CKS_INTCLK_1024_8192, TMR_ICKS_div8192_RE },
  };

  uint32_t pck = clock_peripheral () / 1000000;// MHz
  uint32_t cnt;
  size_t i;

  for (i = 0; i < sizeof div / sizeof div[0]; i++)
    if ((cnt = (pck * usec) / div[i].val) < 0xff)
      break;

  if (i == sizeof div / sizeof div[0])
    return 0;
#ifdef STANDALONE
  printf ("%d: val=%d CKS=%d ICKS=%d cnt=%d\n", usec, div[i].val, div[i].CKS,
	  div[i].ICKS, cnt);
#else
  config->TCCR &= ~TCCR_ICKS_MASK;
  config->TCCR |= div[i].ICKS;
  config->TCR &= ~TCR_CKS_MASK;
  config->TCR |= div[i].CKS;
#endif

  return cnt;
}
