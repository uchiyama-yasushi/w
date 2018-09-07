
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

#ifndef _DEV_I8259_H_
#define	_DEV_I8259_H_

/*
  i8259
  ICW(initialization command words)1-4
  OCW(operation command words)1-3
  ISR In Service Register
  IMR Interrupt Mask Register
  IRR Interrupt Request Register.
  A0 D4 D3
   0  1  0: ICW1 Initialize start.
   1  0  0 : OCW1
   0  0  0 : OCW2
   0  0  1 : OCW3

 */
#define	ICW1_START	0x10
#define	ICW1_IC4	0x01	// ICW4 needed
#define	ICW1_SNGL	0x02	// 1:single, 0:cascade
#define	_ICW1_ADI	0x04	// call address interval (MCS-80/85 only)
#define	ICW1_LTIM	0x08	// trigger 1:level 0: edge
// MCS-80/85 mode only.
#define	_ICW1_A5_7	0xe0	// A7-A5 of interrupt vector address

// MCS-80/85 mode only.
#define	_ICW2_A8_15	0xff	// A8-A15 of interrupt vector address
// 8086/8086 mode.
#define	ICW2_T3_T7	0xf8	// ...8byte align.

#define	ICW3_MASTER_MASK	0xff
#define	ICW3_SLAVE_MASK		0x07

#define	ICW4_uPM	0x01	// 1:8086/8088 0:MCS-80/85
#define	ICW4_AEOI	0x02	// 1:Auto EOI  0:Normal EOI
#define	ICW4_MS		0x04	// 1:Master 0:Slave
#define	ICW4_BUF	0x08	// 1:Bufferd mode.
#define	ICW4_SFNM	0x10	// 1: Special fully nested mode

#define	OCW2		0x00
#define	OCW2_R		0x80
#define	OCW2_SL		0x40
#define	OCW2_EOI	0x20
#define	OCW2_LEVEL_MASK	0x07

#define	OCW3		0x08
// Specail Mask Mode              SSMM  SMM
#define	OCW3_SSMM	0x40	// 1      1 set special mask
#define	OCW3_SMM	0x20	// 1      0 reset special mask
#define	OCW3_P		0x04	// Poll command
#define	OCW3_RR		0x02	// Read Register
#define	OCW3_RIS	0x01	// Read In Service Register
#define	OCW3_READ_IRR	0x02
#define	OCW3_READ_ISR	0x03

__BEGIN_DECLS
void i8259_enable (void);
void i8259_set_mask (uint16_t);
uint16_t i8259_get_mask (void);
void i8259_eoi_master (void);
void i8259_eoi_slave (void);
uint16_t i8259_isr_get (void);
uint8_t i8259_irr_get (void);
__END_DECLS

#endif
