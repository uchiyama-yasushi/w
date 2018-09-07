
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

typedef	unsigned short int     uint16_t;
typedef	unsigned char     uint8_t;

#define	PGCR4	((volatile uint16_t *)0xfffe38c6)

#define	SCSMR_6		((volatile uint16_t *)0xfffeb000)
#define	SCBRR_6		((volatile uint8_t *)0xfffeb004)
#define	SCSCR_6		((volatile uint16_t *)0xfffeb008)
#define	SCFTDR_6	((volatile uint8_t *)0xfffeb00c)
#define	SCFSR_6		((volatile uint16_t *)0xfffeb010)
#define	SCFRDR_6	((volatile uint8_t *)0xfffeb014)
#define	SCFCR_6		((volatile uint16_t *)0xfffeb018)
#define	SCFDR_6		((volatile uint16_t *)0xfffeb01c)
#define	SCSPTR_6	((volatile uint16_t *)0xfffeb020)
#define	SCLSR_6		((volatile uint16_t *)0xfffeb024)
#define	SCEMR_6		((volatile uint16_t *)0xfffeb028)

#define	STBCR4		((volatile uint8_t *)0xfffe040c)

void
startup ()
{
#define	PCIOR0	((volatile uint16_t *)0xfffe3852)
#define	PCCR2	((volatile uint16_t *)0xfffe384a)
#define	PCDR	((volatile uint16_t *)0xfffe3856)

  *PCCR2 &= ~0x3;	// PC8
  *PCIOR0 |= (1 << 8);	// PC8 output
  *PCDR &= ~(1 << 8);

  // SCI6 Power on
  if (*STBCR4 & 1 << 1)
    {
      *STBCR4 &= ~(1 << 1);
      *STBCR4;	// Dummy read. Ensure power on.
    }
  else
    {
      // Already Powered.
      // Drain FIFO
      while (*SCFDR_6 & (0x1f << 8))
	;
      // Wait until exhausted.
      int i;
      for (i = 0; i < 10000; i++)
	;
    }

  // SCI initialize
  *SCSCR_6 &= ~(0xf << 4); // Drop RIE, TIE, TE, RE
  *SCFCR_6 |= (0x3 << 1); // Reset FIFO
  *SCFSR_6 |= 0;	// Clear Status.
  *SCSCR_6 &= ~0x3;	// Internal clock SCK pin is input.
  *SCSMR_6 = 0;	// 8N1 Pck 1/1 (Pck=24MHz)
  *SCEMR_6 = 0;	// x16 base clock
  *SCBRR_6 = 19; // 38400bps
  *SCFCR_6 = 0;	// FIFO

  // Pin configuration
  // TxD6
  *PGCR4 &= ~(0x7 << 8);
  *PGCR4 |= (0x4 << 8);

  // RxD6
  *PGCR4 &= ~(0x7 << 4);
  *PGCR4 |= (0x4 << 4);

  *SCSCR_6 |= (0x3 << 4); // TE,RE

#define	TDFE	(1 << 5)
#define	TEND	(1 << 6)

  while (/*CONSTCOND*/1)
    {
      while (!(*SCFSR_6 & TDFE))
	;
      *SCFTDR_6 = 'A';
      while (!(*SCFSR_6 & (TDFE | TEND)))
	;
      *SCFSR_6 &= ~(TDFE | TEND);
    }
}