
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

#ifndef _LPC23XX_I2C_H_
#define	_LPC23XX_I2C_H_
#define	I2C0_BASE	0xe001c000	// I2C compliant open drain
#define	I2C1_BASE	0xe005c000	// not open drain
#define	I2C2_BASE	0xe0080000	// not open drain

#define	_I2C_CONSET	0x00
#define	_I2C_STAT	0x04
#define	_I2C_DAT	0x08
#define	_I2C_ADR	0x0c
#define	_I2C_SCLH	0x10
#define	_I2C_SLLL	0x14
#define	_I2C_CONCLR	0x18

// I2C Control Set Register
#define	I2C_CONSET(x)	((volatile uint32_t *)(I2C ## x ## _BASE + _I2C_CONSET))
#define	 CONSET_AA		0x04	// Assert Acknowledge
#define	 CONSET_SI		0x08	// Interrupt flag
#define	 CONSET_STO		0x10	// STOP flag
#define	 CONSET_STA		0x20	// START flag
#define	 CONSET_I2EN		0x40	// I2C Interface Enable.
#define	 CONSET_VALID_BITS	0x7c

// I2C Status Register
#define	I2C_STAT(x)	((volatile uint32_t *)(I2C ## x ## _BASE + _I2C_STAT))
#define	 STAT_MASK		0x1f
#define	 STAT_SHIFT		3
/*
  Master Transmitter mode.
  0x08	A START condition has been transmitted.
  0x10	A repeated START condition has been transmitted.
  0x18	SLA+W has been transmitted; ACK has been received.
  0x20	SLA+W has been transmitted; NOT ACK has been received.
  0x28	Data byte in I2DAT has been transmitted; ACK has been received.
  0x30	Data byte in I2DAT has been transmitted; NOT ACK has been received.
  0x38	Arbitration lost in SLA+R/W or Data bytes.

  Master Receiver mode.
  0x08
  0x10
  0x38
  0x40	SLA+R has been transmitted; ACK has been received.
  0x48	SLA+R has been transmitted; NOT ACK has been received.
  0x50	Data byte has been received; ACK has been retruned.
  0x58	Data byte has been received; NOT ACK has been retruned.

  Slave Receiver mode.
  0x60	Own SLA+W has been receivied; ACK has been returned.
  0x68	Arbitration lost in SLA+R/W as master; Own SLA +W has been received, ACK returnd.
  0x70	General call address has been received;ACK has been returned.
  0x78	Arbitration lost in SLA+R/W as master; General call haddress has been received, ACK has been returned.
  0x80	Previously addressed with own SLV address; DATA has been received; ACK has been returned.
  0x88	Previously addressed with own SLV address; DATA has been received; NOT ACK has been returned.
  0x90	Previously addressed with General Call; DATA has been received; ACK has been returned.
  0x98	Previously addressed with General Call; DATA has been received; NOT ACK has been returned.
  0xa0	A STOP condition or repeated START condition has been received while still addressed as SLV/REC or SLV/TRX.

  Slave Transmitter mode.
  0xa8	Own SLA+R has been received; ACK has been returned.
  0xb0	Arbitration lost in SLA+R/W as master; Own SLA+R has been received, ACK has been returned.
  0xb8	Data byte in I2DAT has been transmitted; ACK has been received.
  0xc0	Data byte in I2DAT has been transmitted; NOT ACK has been received.
  0xc8	Last data byte in I2DAT has been transmitted (AA=0);ACK has been received.

  Misc state.
  0xf8	No relavant information is available. (SI is not yet set.)
  0x00	Bus error occured.

 */

// I2C Data Register
#define	I2C_DAT(x)	((volatile uint32_t *)(I2C ## x ## _BASE + _I2C_DAT))

// I2C Slave Address Register
#define	I2C_ADR(x)	((volatile uint32_t *)(I2C ## x ## _BASE + _I2C_ADR))
#define	 ADR_GC			0x01	// General Call enable
#define	 ADR_SLAVE_MASK		0x7f
#define	 ADR_SLAVE_SHIFT	1

// SCH Duty Cycle Register High Half Word
#define	I2C_SCLH(x)	((volatile uint32_t *)(I2C ## x ## _BASE + _I2C_SCLH))

// SCH Duty Cycle Register Low Half Word
#define	I2C_SCLL(x)	((volatile uint32_t *)(I2C ## x ## _BASE + _I2C_SLLL))

// I2C Control Clear Register
#define	I2C_CONCLR(x)	((volatile uint32_t *)(I2C ## x ## _BASE + _I2C_CONCLR))
#define	 CONCLR_AAC		0x04	// Assert acknowledge clear
#define	 CONCLR_SIC		0x08	// I2C interrupt clear
#define	 CONCLR_STAC		0x20	// START flag clear
#define	 CONCLR_I2ENC		0x40	// I2C Interface Disable
#define	 CONCLR_VALID_BITS	0x6c

__BEGIN_DECLS
struct i2c_state;

struct i2c_command
{
  bool success;
  volatile bool busy;
  int n_command;
  int current;
  struct i2c_state *state;
} i2c_command;

enum i2c_control_option
  {
    I2C_CONTINUE,
    I2C_START,
    I2C_STOP,
  };

enum i2c_transfer_dir
  {
    I2C_DIR_WRITE = 0,
    I2C_DIR_READ = 1,
  };

struct i2c_state
{
  uint8_t data;
  uint8_t *buffer;
  enum i2c_control_option option;
  bool (*func)(struct i2c_command *, uint32_t);
  void (*opt_func)(void);
};

#define	I2C_NOTIMEOUT	-1
void i2c0_init (void);
void i2c0_fini (void);
bool i2c0_execute (struct i2c_command *, int);

// Prepare state machine.
void i2c_cmd_init (struct i2c_command *);
void i2c_cmd_slave_addr (struct i2c_command *, uint8_t, uint8_t);
void i2c_cmd_write (struct i2c_command *, uint8_t, enum i2c_control_option);
void i2c_cmd_read (struct i2c_command *, uint8_t *, enum i2c_control_option);
__END_DECLS

#endif
