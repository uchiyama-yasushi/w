
/*-
 * Copyright (c) 2008-2009 UCHIYAMA Yasushi.  All rights reserved.
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

#ifndef _SCI3_H_
#define	_SCI3_H_

  /* Serial Mode Register */
#define	SCI_SMR		((volatile uint8_t *)0xffa8)	// R/W 0x00
#define	SMR_CM		0x80	/* Communication Mode (1:clock)*/
#define	SMR_CHR		0x40	/* Character Length (0:8bit) */
#define	SMR_PE		0x20	/* Parity Enable */
#define	SMR_PM		0x10	/* Parity Mode (0:even) */
#define	SMR_STOP	0x08	/* Stop bit length (0: 1stopbit) */
#define	SMR_MP		0x04	/* Multi Processor mode */
#define	SMR_CKS1	0x02	/* ClocK Select 1 */
#define	SMR_CKS0	0x01	/* ClocK Select 0 */
  /* Bit Rate Register */
#define	SCI_BRR		((volatile uint8_t *)0xffa9)	// R/W 0xff
  /* Serial Control Register */
#define	SCI_SCR3	((volatile uint8_t *)0xffaa)	// R/W 0x00
#define	SCR_TIE		0x80	/* Transmit Interrupt Enable */
#define	SCR_RIE		0x40	/* Receive Interrupt Enable */
#define	SCR_TE		0x20	/* Transmit Enable */
#define	SCR_RE		0x10	/* Receive Enable */
#define	SCR_MPIE	0x08	/* Multi Processor Interrupt Enable */
#define	SCR_TEIE	0x04	/* Transmit End Interrupt Enable */
#define	SCR_CKE1	0x02	/* ClocK Enable 1 */
#define	SCR_CKE0	0x01	/* ClocK Enable 0 */
  /* Transmit Data Register */
#define	SCI_TDR		((volatile uint8_t *)0xffab)	// R/W 0xff
  /* Serial Status Register */
#define	SCI_SSR		((volatile uint8_t *)0xffac)	// R/(W) 0x84 0 write only
#define	SSR_TDRE	0x80	/* Transmit Data Register Empty */
#define	SSR_RDRF	0x40	/* Receive Data Register Full */
#define	SSR_ORER	0x20	/* OverRun ERror */
#define	SSR_FER		0x10	/* Framing ERror */
#define	SSR_PER		0x08	/* Parity ERror */
#define	SSR_TEND	0x04	/* Transmit END */
#define	SSR_MPB		0x02	/* MultiProcessor Bit */
#define	SSR_MPBT	0x01	/* MultiProcessor Bit Transfer */
#define	SSR_ERR_BITS	(SSR_ORER | SSR_FER | SSR_PER)
  /* Receive Data Register */
#define	SCI_RDR		((volatile uint8_t *)0xffad)	// R 0x00
/*
  CKS1 CKS0 clock
   0    0    1/1
   0    1    1/4
   1    0    1/16
   1    1    1/64

   BRR = clk / (64 *  2 ^ (2*n -1) * B) - 1
   B: bit/s
   clk (Hz)
   n SMR CKS1, CKS0 (0 <= n <= 3)

   CKS1=CKS=0
   BRR = clk / (32*B) - 1

--------16MHz--------
  9600 51.083
 19200 25.042
 38400 12.021
 57600 7.681
115200 3.340
--------20MHz--------
  9600 64.104
 19200 31.552
 38400 15.276
 57600 9.851
115200 4.425

#include <float.h>
#include <stdio.h>

main ()
{
  double brr;
  double bps[] = { 9600., 19200., 38400., 57600., 115200. };
  int clock[] = { 16, 20 };
  double clk;
  int i, j;

  for (j = 0; j < sizeof clock / sizeof (clock[0]); j++)
    {
      clk = clock[j] * 1000000;
      printf ("--------%dMHz--------\n", clock[j]);
      for (i = 0; i < sizeof bps / sizeof (bps[0]); i++)
	{
	  brr = clk / (32 * bps[i]) -1;
	  printf ("%6.0f %3.3f\n", bps[i], brr);
	}
    }
}

*/
#endif //_SCI3_H_
