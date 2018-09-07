
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

// Timer W

#define	TMRW	((volatile uint8_t *)0xff80)
#define	 TMRW_CTS	0x80
#define	 TMRW_BUFEB	0x20
#define	 TMRW_BUFEA	0x10
#define	 TMRW_PWMD	0x04
#define	 TMRW_PWMC	0x02
#define	 TMRW_PWMB	0x01

////////////////////////////////////////
// Timer Control Register W
#define	TCRW	((volatile uint8_t *)0xff81)
// Counter CLeaR
#define	 TCRW_CCLR	0x80
// ClocK Select
#define	 _TCRW_CKS2	0x40
#define	 _TCRW_CKS1	0x20
#define	 _TCRW_CKS0	0x10
#define	 TCRW_CKS_DIV1	0
#define	 TCRW_CKS_DIV2	_TCRW_CKS0
#define	 TCRW_CKS_DIV4	_TCRW_CKS1
#define	 TCRW_CKS_DIV8	(_TCRW_CKS0 | _TCRW_CKS1)
#define	 TCRW_CKS_EXT	_TCRW_CKS2
/*
clock divider   MHz     usec/clock      overflow(msec)
16	 1	16.00	0.062500	4.096000
16	 2	8.00	0.125000	8.192000
16	 4	4.00	0.250000	16.384001
16	 8	2.00	0.500000	32.768002
20	 1	20.00	0.050000	3.276800
20	 2	10.00	0.100000	6.553600
20	 4	5.00	0.200000	13.107201
20	 8	2.50	0.400000	26.214401

*/
// Timer Output level set [A-D]
#define	 TCRW_TOD	0x08
#define	 TCRW_TOC	0x04
#define	 TCRW_TOB	0x02
#define	 TCRW_TOA	0x01

////////////////////////////////////////
// Timer Interrupt Enable Register W
#define	TIERW	((volatile uint8_t *)0xff82)
// OVerflow Interrupt Enable
#define	 TIERW_OVIE	0x80
// Input capture/ compare Match Interrupt Enable [A-D]
#define	 TIERW_IMIED	0x08
#define	 TIERW_IMIEC	0x04
#define	 TIERW_IMIEB	0x02
#define	 TIERW_IMIEA	0x01


////////////////////////////////////////
// Timer Status Register W
#define	TSRW	((volatile uint8_t *)0xff83)
// OVerFlow
#define	 TSRW_OVF	0x80
// Input capture/ compare Match [A-D]
#define	 TSRW_IMFD	0x08
#define	 TSRW_IMFC	0x04
#define	 TSRW_IMFB	0x02
#define	 TSRW_IMFA	0x01

////////////////////////////////////////
// Timer I/O control Register 0
#define	TIOR0	((volatile uint8_t *)0xff84)
#define	 TIOR0_IOB2	0x40
#define	 TIOR0_IOB1	0x20
#define	 TIOR0_IOB0	0x10
#define	 TIOR0_IOA2	0x04
#define	 TIOR0_IOA1	0x02
#define	 TIOR0_IOA0	0x01

////////////////////////////////////////
#define	TIOR1	((volatile uint8_t *)0xff85)
#define	 TIOR1_IOD2	0x40
#define	 TIOR1_IOD1	0x20
#define	 TIOR1_IOD0	0x10
#define	 TIOR1_IOC2	0x04
#define	 TIOR1_IOC1	0x02
#define	 TIOR1_IOC0	0x01

#define	TCNT	((volatile uint16_t *)0xff86)
#define	GRA	((volatile uint16_t *)0xff88)
#define	GRB	((volatile uint16_t *)0xff8a)
#define	GRC	((volatile uint16_t *)0xff8c)
#define	GRD	((volatile uint16_t *)0xff8e)

/*
main ()
{
  float divider[] = {1., 2., 4., 8. };
  float clock[] = { 16, 20 };
  int overflow = 65536;
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
