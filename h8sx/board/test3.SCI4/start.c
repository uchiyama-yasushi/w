
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

#define	PM3_DDR	((volatile uint8_t *)0xffee50)
#define	PM3_DR	((volatile uint8_t *)0xffee51)

#define	_SCI4_BASE	0xfffe90
/* Serial Mode Register R/W */
#define	_SCI_SMR	0
#define	SMR_CM		0x80	/* Communication Mode (1:clock)*/
#define	SMR_CHR		0x40	/* Character Length (0:8bit) */
#define	SMR_PE		0x20	/* Parity Enable */
#define	SMR_PM		0x10	/* Parity Mode (0:even) */
#define	SMR_STOP	0x08	/* Stop bit length (0: 1stopbit) */
#define	SMR_MP		0x04	/* Multi Processor mode */
#define	SMR_CKS1	0x02	/* ClocK Select 1 */
#define	SMR_CKS0	0x01	/* ClocK Select 0 */
/* Bit Rate Register */
#define	_SCI_BRR	1
/* Serial Control Register R/W */
#define	_SCI_SCR	2
#define	SCR_TIE		0x80	/* Transmit Interrupt Enable */
#define	SCR_RIE		0x40	/* Receive Interrupt Enable */
#define	SCR_TE		0x20	/* Transmit Enable */
#define	SCR_RE		0x10	/* Receive Enable */
#define	SCR_MPIE	0x08	/* Multi Processor Interrupt Enable */
#define	SCR_TEIE	0x04	/* Transmit End Interrupt Enable */
#define	SCR_CKE1	0x02	/* ClocK Enable 1 */
#define	SCR_CKE0	0x01	/* ClocK Enable 0 */
/* Transmit Data Register R/W */
#define	_SCI_TDR	3
/* Serial Status Register R/(W) */
#define	_SCI_SSR	4
#define	SSR_TDRE	0x80	/* Transmit Data Register Empty (0 write only)*/
#define	SSR_RDRF	0x40	/* Receive Data Register Full (0 write only)*/
#define	SSR_ORER	0x20	/* OverRun ERror (0 write only)*/
#define	SSR_FER		0x10	/* Framing ERror (0 write only)*/
#define	SSR_PER		0x08	/* Parity ERror (0 write only)*/
#define	SSR_TEND	0x04	/* Transmit END (read only)*/
#define	SSR_MPB		0x02	/* MultiProcessor Bit (read only)*/
#define	SSR_MPBT	0x01	/* MultiProcessor Bit Transfer */

#define	SSR_ERR_BITS	(SSR_ORER | SSR_FER | SSR_PER)
/* Receive Data Register */
#define	_SCI_RDR	5
/* Smart Card Mode Register */
#define	_SCI_SCMR	6
#define	SCMR_SDIR	0x08
#define	SCMR_SINV	0x04
#define	SCMR_SMIF	0x01	/* SMart card InterFace Select */

#define	SCI(n, r)	((volatile uint8_t *)_SCI ## n ## _BASE + _SCI_ ## r)

/* Input buffer Control Register  1:Enable, 0:Disable */
#define	P6_ICR		((volatile uint8_t *)0xfffb95)

/* Module SToP Control Register B */
#define	MSTPCRB		((volatile uint16_t *)0xfffdca)


void sci4_init (void);
int8_t sci4_getc (void);
void sci4_putc (int8_t);

void
machine_startup ()
{
  int8_t c;

  *PM3_DDR |= 8;
  *PM3_DR &= ~8;	// LED on.

  sci4_init ();
  sci4_putc ('o');
  sci4_putc ('h');
  sci4_putc ('a');
  sci4_putc ('y');
  sci4_putc ('o');
  sci4_putc ('\r');
  sci4_putc ('\n');
  while ((c = sci4_getc ()))
    sci4_putc (c);

  // Error.
  *PM3_DR |= 8;		// LED off.
    ;
  /*NOTREACHED*/
}

void
sci4_init ()
{
  // Start SCI4 module
  *MSTPCRB &= ~(1 << 12);

  // Disable transmit/receive.
  *SCI (4, SCR) &= ~(SCR_TE | SCR_RE);

  // Setup Input buffer. Use internal clock.
  *P6_ICR &= ~1;	//P60: SCI4 TxD
  *P6_ICR |= 2;		//P61: SCI4 RxD

  // Clock select
  *SCI (4, SCR) &= ~(SCR_CKE0 | SCR_CKE1); // Peripheral clock / 1

  // Tx/Rx Format.
  *SCI (4, SCMR) = 0;	// Clocked synchronous mode.
  *SCI (4, SMR)	= 0;	// 8N1

  // Bit rate.
  // Input Clock: 12MHz
  // CPU clock: 12MHz
  // Peripheral clock 12MHz
  // 12.* 1000000/(64/2 *38400) -1 = 8.77
  *SCI (4, BRR) = 9;	//38400bps

  // Wait at least 1bit interval.
  volatile int i;
  for (i = 0; i < 0xffff; i++)
    ;

  // Enable transmit/receive.
  *SCI (4, SCR) |= (SCR_TE | SCR_RE);
}

int8_t
sci4_getc ()
{
  uint8_t c;

  while (((c = *SCI (4, SSR)) & (SSR_RDRF | SSR_ERR_BITS)) == 0)
    ;
  if (c & SSR_ERR_BITS)
    {
      *SCI (4, SSR) &= ~(SSR_RDRF | SSR_ERR_BITS);
      return 0;
    }

  c = *SCI (4, RDR);
  *SCI (4, SSR) &= ~SSR_RDRF;

  return c;
}

void
sci4_putc (int8_t c)
{

  while ((*SCI (4, SSR) & SSR_TDRE) == 0)
    ;
  *SCI (4, TDR) = c;
  *SCI (4, SSR) &= ~SSR_TDRE;

  // Wait transfer complete.
  while ((*SCI (4, SSR) & SSR_TEND))
    ;
}
