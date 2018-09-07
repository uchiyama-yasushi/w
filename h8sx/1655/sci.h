
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
// H8SX/1655 SCI module.

#ifndef _H8SX_1655_SCI_H_
#define	_H8SX_1655_SCI_H_

#define	_SCI4_BASE	0xfffe90
#define	_SCI6_BASE	0xfff610

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


#endif
