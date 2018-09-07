
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

// Akizuki AEKXP84-2050 Accelerometer

#include <sys/system.h>
#include <sys/console.h>
#include <string.h>
#include <reg.h>

#include <device/kxp84.h>
#include <device/kxp84_i2c.h>

//#define	SINGLE_TRANSFER
void kxp84_reg_read_1 (struct i2c_command *, uint8_t, uint8_t *);
#ifdef SINGLE_TRANSFER
void kxp84_reg_write_1 (struct i2c_command *, uint8_t, uint8_t);
#else
void kxp84_reg_write_6 (struct i2c_command *, uint8_t, uint8_t *);
void kxp84_reg_read_6 (struct i2c_command *, uint8_t, uint8_t *);
#endif

STATIC struct i2c_command rdcmd;
STATIC struct i2c_state rdst[16];
STATIC uint8_t *__user_buffer;
STATIC void (*reset_func) (void);

bool
kxp84_init (void (*reset)(void))
{
  struct i2c_command wrcmd;
  struct i2c_state wrst[16];
  uint8_t buf[8], check_buf[8];

  reset_func = reset;
  wrcmd.state = wrst;
  rdcmd.state = rdst;

  if (reset_func)
    reset_func ();

  i2c0_init ();

  // Control register setting.
  buf[0] = 0x14;	// FF_INT	0.4g
  buf[1] = 0x14;	// FF_DELAY	80msec
  buf[2] = 0x4d;	// MOT_INT	1.5g
  buf[3] = 0x14;	// MOT_DELAY	80msec
  buf[4] = REGC_FREQ_250Hz;	// CTRL_REGC
  buf[5] = REGB_CLKhld;		// CTRL_REGB
  // CTRL_REGA is read only.
#ifdef SINGLE_TRANSFER
  int i;
  for (i = 0; i < 6; i++)
    {
      kxp84_reg_write_1 (&wrcmd, KXP84_REG_FF_INT + i, buf[i]);
      i2c0_execute (&wrcmd, I2C_NOTIMEOUT);
    }
#else
  kxp84_reg_write_6 (&wrcmd, KXP84_REG_FF_INT, buf);
  i2c0_execute (&wrcmd, I2C_NOTIMEOUT);
#endif

  // Read Control register [0x6:0xb] for check.
#ifdef SINGLE_TRANSFER
  for (i = 0; i < 7; i++)
    {
      kxp84_reg_read_1 (&rdcmd, KXP84_REG_FF_INT + i, check_buf + i);
      i2c0_execute (&rdcmd, I2C_NOTIMEOUT);
    }
#else
  kxp84_reg_read_6 (&rdcmd, KXP84_REG_FF_INT, check_buf);
  i2c0_execute (&rdcmd, I2C_NOTIMEOUT);
  kxp84_reg_read_1 (&rdcmd, KXP84_REG_REGA, check_buf + 6);
  i2c0_execute (&rdcmd, I2C_NOTIMEOUT);
#endif
#ifdef DEBUG
  int j;
  iprintf ("Control register:");
  for (j = 0; j < 7; j++)
    {
      iprintf ("[%x:%x] ", check_buf[j], buf[j]);
    }
  iprintf ("\n");
#endif
  if (memcmp (buf, check_buf, 6) != 0)
    {
#ifdef DEBUG
      iprintf ("KXP84 initialize failed.\n");
#endif
      return FALSE;
    }

  return TRUE;
}

void
kxp84_read_setup (uint8_t *buf)
{
#ifndef SINGLE_TRANSFER
  kxp84_reg_read_6 (&rdcmd, KXP84_REG_XOUT_H, buf);
#endif
  __user_buffer = buf;
}

bool
kxp84_read (uint16_t *x, uint16_t *y, uint16_t *z)
{
#ifdef SINGLE_TRANSFER
  int i;
  for (i = 0; i < 6; i++)
    {
      kxp84_reg_read_1 (&rdcmd, i, __user_buffer + i);
      if (!i2c0_execute (&rdcmd, I2C_NOTIMEOUT))
	return FALSE;
    }
#else
  // KXP84 A/D coversation time is 200usec. 1msec is enough for preparing data.
  if (!i2c0_execute (&rdcmd, 1/*msec*/))
    return FALSE;
#endif

  *x = KXP84_ACCEL_DATA (__user_buffer[0], __user_buffer[1]);
  *y = KXP84_ACCEL_DATA (__user_buffer[2], __user_buffer[3]);
  *z = KXP84_ACCEL_DATA (__user_buffer[4], __user_buffer[5]);

  return TRUE;
}

void
kxp84_reg_write_1 (struct i2c_command *cmd, uint8_t reg_addr, uint8_t reg_data)
{
  // Prepare state machine.
  i2c_cmd_init (cmd);
  i2c_cmd_slave_addr (cmd, KXP84_I2C_ADDR0, I2C_DIR_WRITE);
  i2c_cmd_write (cmd, reg_addr, I2C_CONTINUE);
  i2c_cmd_write (cmd, reg_data, I2C_STOP);
}

void
kxp84_reg_write_6 (struct i2c_command *cmd, uint8_t reg_addr, uint8_t *reg_data)
{
  // Prepare state machine.
  i2c_cmd_init (cmd);
  i2c_cmd_slave_addr (cmd, KXP84_I2C_ADDR0, I2C_DIR_WRITE);
  i2c_cmd_write (cmd, reg_addr, I2C_CONTINUE);
  i2c_cmd_write (cmd, *reg_data++, I2C_CONTINUE);
  // KXP84 automatically increments the received register address.
  i2c_cmd_write (cmd, *reg_data++, I2C_CONTINUE);
  i2c_cmd_write (cmd, *reg_data++, I2C_CONTINUE);
  i2c_cmd_write (cmd, *reg_data++, I2C_CONTINUE);
  i2c_cmd_write (cmd, *reg_data++, I2C_CONTINUE);
  i2c_cmd_write (cmd, *reg_data++, I2C_STOP);
}

void
kxp84_reg_read_1 (struct i2c_command *cmd, uint8_t reg_addr, uint8_t *reg_data)
{
  // Prepare state machine.
  i2c_cmd_init (cmd);
  i2c_cmd_slave_addr (cmd, KXP84_I2C_ADDR0, I2C_DIR_WRITE);
  i2c_cmd_write (cmd, reg_addr, I2C_START);
  i2c_cmd_slave_addr (cmd, KXP84_I2C_ADDR0, I2C_DIR_READ);
  i2c_cmd_read (cmd, reg_data, I2C_STOP);
}

void
kxp84_reg_read_6 (struct i2c_command *cmd, uint8_t reg_addr, uint8_t *reg_data)
{
  // Prepare state machine.
  i2c_cmd_init (cmd);
  i2c_cmd_slave_addr (cmd, KXP84_I2C_ADDR0, I2C_DIR_WRITE);
  i2c_cmd_write (cmd, reg_addr, I2C_START);
  i2c_cmd_slave_addr (cmd, KXP84_I2C_ADDR0, I2C_DIR_READ);
  i2c_cmd_read (cmd, reg_data, I2C_CONTINUE);
  // KXP84 automatically increments the received register address.
  i2c_cmd_read (cmd, reg_data + 1, I2C_CONTINUE);
  i2c_cmd_read (cmd, reg_data + 2, I2C_CONTINUE);
  i2c_cmd_read (cmd, reg_data + 3, I2C_CONTINUE);
  i2c_cmd_read (cmd, reg_data + 4, I2C_CONTINUE);
  i2c_cmd_read (cmd, reg_data + 5, I2C_STOP);
}
