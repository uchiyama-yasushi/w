
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

// H8/3664 I2C interface.
#ifndef _I2C_H_
#define	_I2C_H_

// I2C Bus Control Register
#define	I2C_ICCR			((volatile uint8_t *)0xffc4)
#define	 ICCR_ICE	0x80	// Bus Interface enable.
#define	 ICCR_IEIC	0x40	// Interrupt enable
#define	 ICCR_MST	0x20	// Master/Slave
#define	 ICCR_TRS	0x10	// Transfer/Receive
#define	 ICCR_ACKE	0x08	// Ack
#define	 ICCR_BBSY	0x04	// Bus busy
#define	 ICCR_IRIC	0x02	// Interrupt request flag.
#define	 ICCR_SCP	0x01

// I2C Bus Status Register
#define	I2C_ICSR			((volatile uint8_t *)0xffc5)
#define	 ICSR_ESTP	0x80	// Error stop
#define	 ICSR_STOP	0x40	// Stop
#define	 ICSR_IRTR	0x20
#define	 ICSR_AASX	0x10
#define	 ICSR_AL	0x08	// Arbitration lost.
#define	 ICSR_AAS	0x04
#define	 ICSR_ADZ	0x02
#define	 ICSR_ACKB	0x01	// Ack bit.

// I2C Bus Data Register
#define	I2C_ICDR			((volatile uint8_t *)0xffc6)

// Slave Address Register
#define	I2C_SAR				((volatile uint8_t *)0xffc7)
#define	 SAR_SVA_SHIFT		1
#define	 SAR_SVA_MASK		0x7f
#define	 SAR_FS			0x01
// Second Slave Address Register
#define	I2C_SARX			((volatile uint8_t *)0xffc6)
#define	 SARX_SVAX_SHIFT	1
#define	 SARX_SVAX_MASK		0x7f
#define	 SAR_FSX		0x01
/*
  FS	FSX
   0	0	I2C-mode SAR and SRX are slave address.
   0	1	I2C-mode SAR is slave address.
   1	0	I2C-mode SARX is slave address.
   1	1	clock synchronized serial mode.
 */

// I2C Bus Mode Register
#define	I2C_ICMR			((volatile uint8_t *)0xffc7)
// MSB/LSB first.
#define	 ICMR_MLS	0x80	// 0: MSB 1st, 1: LSB 1st.
// Wait insert
#define	 ICMR_WAIT	0x40
// Transfer clock *(with TSCR IICX bit)*
#define	 ICMR_CKS2	0x20
#define	 ICMR_CKS1	0x10
#define	 ICMR_CKS0	0x08
// Bit counter
#define	 ICMR_BC2	0x04
#define	 ICMR_BC1	0x02
#define	 ICMR_BC0	0x01
// I2C-format
#define	 ICMR_I2C_9BIT	(0)
#define	 ICMR_I2C_2BIT	(ICMR_BC0)
#define	 ICMR_I2C_3BIT	(ICMR_BC1)
#define	 ICMR_I2C_4BIT	(ICMR_BC0 | ICMR_BC1)
#define	 ICMR_I2C_5BIT	(ICMR_BC2)
#define	 ICMR_I2C_6BIT	(ICMR_BC2 | ICMR_BC0)
#define	 ICMR_I2C_7BIT	(ICMR_BC2 | ICMR_BC1)
#define	 ICMR_I2C_8BIT	(ICMR_BC2 | ICMR_BC1 | ICMR_BC0)
// Clock synchronaized serial format
#define	 ICMR_CLK_8BIT	(0)
#define	 ICMR_CLK_1BIT	(ICMR_BC0)
#define	 ICMR_CLK_2BIT	(ICMR_BC1)
#define	 ICMR_CLK_3BIT	(ICMR_BC0 | ICMR_BC1)
#define	 ICMR_CLK_4BIT	(ICMR_BC2)
#define	 ICMR_CLK_5BIT	(ICMR_BC2 | ICMR_BC0)
#define	 ICMR_CLK_6BIT	(ICMR_BC2 | ICMR_BC1)
#define	 ICMR_CLK_7BIT	(ICMR_BC2 | ICMR_BC1 | ICMR_BC0)

// Timer Serial Control Register
#define	I2C_TSCR			((volatile uint8_t *)0xfffc)
// Reset I2C control unit.
#define	 TSCR_IIRST	0x02
// trasfer rate *(with ICMR CKS0-CKS2)*
#define	 TSCR_IICX	0x01
__BEGIN_DECLS
void i2c_init (void);
void i2c_start (void);	// Start condition.
void i2c_repeated_start (void);	// Repeated start condition.
void i2c_stop (void);	// Stop condition
bool i2c_write (uint8_t);	// write 1byte.
void i2c_read_n (uint8_t *, int);
void i2c_status (const char *);

__END_DECLS
#endif
