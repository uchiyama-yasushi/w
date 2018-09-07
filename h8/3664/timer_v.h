
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

// H8/3664 Timer V

// Timer V

#define	TCNTV	((volatile uint8_t *)0xffa4)
#define	TCORA	((volatile uint8_t *)0xffa2)
#define	TCORB	((volatile uint8_t *)0xffa3)

#define	TCRV0	((volatile uint8_t *)0xffa0)
#define	 TCRV0_CMIEB	0x80	// Interrupt Enable B
#define	 TCRV0_CMIEA	0x40	// Interrupt Enable A
#define	 TCRV0_OVIE	0x20	// Interrupt Enable Overflow
#define	 _TCRV0_CCLR1	0x10
#define	 _TCRV0_CCLR0	0x08
#define	  TCRV0_CLR_NONE	0
#define	  TCRV0_CLR_A		_TCRV0_CCLR0
#define	  TCRV0_CLR_B		_TCRV0_CCLR1
#define	  TCRV0_CLR_EXTPIN	(_TCRV0_CCLR1 | _TCRV0_CCLR0)
#define	 _TCRV0_CKS2	0x04
#define	 _TCRV0_CKS1	0x02
#define	 _TCRV0_CKS0	0x01
//							       TCRV1 ICKS0
#define	 TCRV0_CKS_NONE		0
#define	 TCRV0_CKS_DIV4INT	_TCRV0_CKS0			// 0
#define	 TCRV0_CKS_DIV8INT	_TCRV0_CKS0			// 1
#define	 TCRV0_CKS_DIV16INT	_TCRV0_CKS1			// 0
#define	 TCRV0_CKS_DIV32INT	_TCRV0_CKS1			// 1
#define	 TCRV0_CKS_DIV64INT	(_TCRV0_CKS1|_TCRV0_CKS0)	// 0
#define	 TCRV0_CKS_DIV128INT	(_TCRV0_CKS1|_TCRV0_CKS0)	// 1
#define	 TCRV0_CKS_RISE_EXT	(_TCRV0_CKS2|_TCRV0_CKS0)
#define	 TCRV0_CKS_FALL_EXT	(_TCRV0_CKS2|_TCRV0_CKS1)
#define	 TCRV0_CKS_BOTH_EXT	(_TCRV0_CKS2|_TCRV0_CKS1|_TCRV0_CKS0)
/*
clock divider   MHz     usec/clock      overflow(msec)
16	 1	16.00	0.062500	0.016000
16	 4	4.00	0.250000	0.064000
16	 8	2.00	0.500000	0.128000
16	16	1.00	1.000000	0.256000
16	32	0.50	2.000000	0.512000
16	64	0.25	4.000000	1.024000
16	128	0.12	8.000000	2.048000

20	 1	20.00	0.050000	0.012800
20	 4	5.00	0.200000	0.051200
20	 8	2.50	0.400000	0.102400
20	16	1.25	0.800000	0.204800
20	32	0.62	1.600000	0.409600
20	64	0.31	3.200000	0.819200
20	128	0.16	6.400000	1.638400
*/

#define	TCSRV	((volatile uint8_t *)0xffa1)
#define	 TCSRV_CMFB	0x80
#define	 TCSRV_CMFA	0x40
#define	 TCSRV_OVF	0x20
#define	 TCSRV_OS3	0x08
#define	 TCSRV_OS2	0x04
#define	 TCSRV_OS1	0x02
#define	 TCSRV_OS0	0x01

#define	TCRV1	((volatile uint8_t *)0xffa5)
#define	 TCRV1_TVEG1	0x10
#define	 TCRV1_TVEG0	0x08
#define	 TCRV1_TRGE	0x04
#define	 TCRV1_ICKS0	0x01
/*
#include <float.h>
#include <stdio.h>

main ()
{
  float divider[] = {1, 4, 8, 16, 32, 64, 128 };
  float clock[] = { 16, 20 };
  int overflow = 256;
  int i, j;

  printf ("clock divider   MHz     usec/clock      overflow(msec)\n");
  for (j = 0; j < sizeof clock / sizeof (clock[0]); j++)
    {
      float clk = clock [j];
      for (i = 0; i < sizeof divider / sizeof (divider[0]); i++)
	{
	  float div = divider [i];
	  printf ("%2.0f\t%2.0f\t%2.2f\t%f\t%f\n",
		  clk, div, clk / div, div / clk, div / clk * overflow/ 1000);
	}
    }
}
*/
