
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

// I2C module.
#include <sys/system.h>
#include <sys/console.h>
#include <sys/delay.h>
#include <reg.h>

#define	I2C_DEBUG

int i2c_debug;
#ifdef I2C_DEBUG
#define	DPRINTF(fmt, args...)	if (i2c_debug) \
    iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

STATIC struct i2c_command *__protocol;

STATIC bool __i2c_slave_addr (struct i2c_command *, uint32_t);
STATIC bool __i2c_write (struct i2c_command *, uint32_t);
STATIC bool __i2c_read (struct i2c_command *, uint32_t);
STATIC void __i2c_start (void);
STATIC void __i2c_stop (void);
STATIC void __i2c_continue (void);
STATIC void i2c_cmd_set_opt_func (struct i2c_state *, enum i2c_control_option);

void
i2c0_init ()
{
  // I2C0

  mcu_peripheral_power (PCONP_PCI2C0, TRUE);
  mcu_peripheral_clock (PCLK_I2C0, CCLK4);	//72/4=18MHz
  // Set pin to I2C function.
  mcu_pin_select (0, 27, 1); //SDA0
  mcu_pin_select (0, 28, 1); //SCL0

  // Setting the I2C data rate and duty cycle.
  // 18MHz / (22+23) = 400KHz
  *I2C_SCLH (0) = 22;
  *I2C_SCLL (0) = 23;

  // Disable I2C module with clear pending status.
  *I2C_CONCLR (0) = CONCLR_VALID_BITS;

  // Interrupt Enable
  vic_interrupt_enable (VIC_I2C0, VIC_IRQ);

  // Enable I2C interface.
  *I2C_CONSET (0) = CONSET_I2EN;
  udelay (100);
}

void
i2c0_fini ()
{

  // Disable I2C module.
  *I2C_CONCLR (0) = CONCLR_VALID_BITS;
  // Power down.
  mcu_peripheral_power (PCONP_PCI2C0, FALSE);
}

void
i2c_cmd_set_opt_func (struct i2c_state *ctx, enum i2c_control_option opt)
{

  switch (opt)
    {
    case I2C_CONTINUE:
      ctx->opt_func = __i2c_continue;
      break;
    case I2C_START:
      ctx->opt_func = __i2c_start;
      break;
    case I2C_STOP:
      ctx->opt_func = __i2c_stop;
      break;
    }
}

void
__i2c_start ()
{
  DPRINTF ("\n");

  *I2C_CONSET (0) = CONSET_STA;
}

void
__i2c_stop ()
{
  DPRINTF ("\n");

  *I2C_CONSET (0) = CONSET_STO;
}

void
__i2c_continue ()
{
  DPRINTF ("\n");
  // Nothing to do.
}

void
i2c_cmd_init (struct i2c_command *cmd)
{

  cmd->current = 0;
  cmd->n_command = 0;
  cmd->busy = FALSE;
}

void
i2c_cmd_slave_addr (struct i2c_command *cmd, uint8_t slave_addr, uint8_t dir)
{
  struct i2c_state *state = cmd->state + cmd->current++;
  cmd->n_command = cmd->current;
  assert (state);

  state->func = __i2c_slave_addr;
  state->data = (slave_addr << 1) | dir;
}

bool
__i2c_slave_addr (struct i2c_command *cmd, uint32_t status)
{
  struct i2c_state *state = cmd->state + cmd->current++;

  if (!(status == 0x08 ||	// START condition.
	status == 0x10))	// Repeated START condition.
    return FALSE;

  DPRINTF ("slave=%x dir=%x\n", state->data >> 1, state->data & 1);
  // Set Slave address and transfer direction.
  *I2C_DAT (0) = state->data;

  return TRUE;
}

void
i2c_cmd_write (struct i2c_command *cmd, uint8_t data,
	       enum i2c_control_option opt)
{
  struct i2c_state *state = cmd->state + cmd->current++;
  cmd->n_command = cmd->current;
  assert (state);

  state->func = __i2c_write;
  state->data = data;
  i2c_cmd_set_opt_func (state, opt);
}

bool
__i2c_write (struct i2c_command *cmd, uint32_t status)
{
  struct i2c_state *state = cmd->state + cmd->current;

  if (status == 0x20)	// after SLA+W's NOT ACK
    {
      // Repeated start condition will be asserted.
      // repeated from SLA+W
      cmd->current--;
      return TRUE;
    }

  if (status != 0x18 &&	// after SLA+W's ACK.
      status != 0x28)	// previous data transfer's ACK.
    return FALSE;
  // XXX and 0x30 for after DATA's NOT ACK. restart from SLA+W'

  if (status == 0x18)
    {
      // Clear Start condition.
      *I2C_CONCLR (0) = CONCLR_STAC;
      *I2C_DAT (0) = state->data;
      // don't increment cmd->current. wait until next ACK
      DPRINTF ("data=%x\n", state->data);
    }
  else
    {
      cmd->current++;
      if (state->opt_func == __i2c_continue)	// previous transmit.
	{
	  *I2C_DAT (0) = cmd->state[cmd->current].data;
	  DPRINTF ("data=%x\n", cmd->state[cmd->current].data);
	}
      else
	{
	  state->opt_func ();	// start or stop or continue.
	}
    }

  return TRUE;
}

void
i2c_cmd_read (struct i2c_command *cmd, uint8_t *buffer,
	      enum i2c_control_option opt)
{
  struct i2c_state *state = cmd->state + cmd->current++;
  cmd->n_command = cmd->current;
  assert (state);

  state->func = __i2c_read;
  state->buffer = buffer;
  i2c_cmd_set_opt_func (state, opt);
}

void
__i2c_issue_ack (struct i2c_command *cmd)
{
  if (cmd->state[cmd->current].opt_func == __i2c_continue)
    *I2C_CONSET (0) = CONSET_AA;	// next ACK
  else
    *I2C_CONCLR (0) = CONCLR_AAC;	// next NOT ACK
}

bool
__i2c_read (struct i2c_command *cmd, uint32_t status)
{
  struct i2c_state *state = cmd->state + cmd->current;

  if (status == 0x48)	// after SLA+R but NOT ACK.
    {
      cmd->current--;	// retry SLA+R
      return TRUE;
    }

  if (!(status == 0x40 ||	// after SLA+R's ACK
	status == 0x50 ||	// will return ACK
	status == 0x58))	// will return NOT ACK
    return FALSE;
  assert (state->buffer);

  switch (status)
    {
    case 0x40:
      // Clear Start condition.
      *I2C_CONCLR (0) = CONCLR_STAC;
      __i2c_issue_ack (cmd);
      break;

    case 0x50:	// ACK (continue)
      *state->buffer = *I2C_DAT (0);
      cmd->current++;
      __i2c_issue_ack (cmd);
      break;
    case 0x58:	// NACK (last)
      cmd->current++;
      *state->buffer = *I2C_DAT (0);
      state->opt_func ();
    }
  DPRINTF ("data=%x\n", *state->buffer);

  return TRUE;
}

void
i2c_status ()
{
#ifdef I2C_DEBUG
  uint32_t status = *I2C_STAT (0);
  uint32_t con = *I2C_CONSET (0);
  DPRINTF ("%s %x %s %s %s\n", __FUNCTION__, status,
	   con & CONSET_STA ? "STA" : "",
	   con & CONSET_STO ? "STO" : "",
	   con & CONSET_AA ? "AA" : ""
	   );
#endif
}

bool
i2c0_execute (struct i2c_command *cmd, int timeout/*msec*/)
{
  timeout = timeout;
  cpu_status_t s = intr_suspend ();
  assert (!cmd->busy);
  __protocol = cmd;
  __protocol->success = FALSE;
  __protocol->current = 0;
  __protocol->busy = TRUE;
  intr_resume (s);

  DPRINTF ("kick!\n");
  i2c_status ();
  __i2c_start ();
  //XXX timeout notyet.
  while (cmd->busy)
    ;
  s = intr_suspend ();
  __protocol = NULL;
  intr_resume (s);

  return cmd->success;
}

void
i2c0_intr ()
{
  uint32_t status = *I2C_STAT (0);

  if (status == 0xf8 ||	// SI is not yet set.
      status == 0x00 || // Bus error occured.
      !__protocol ||	// No execute context.
      !__protocol->busy)// Not execute state.
    {
      iprintf ("[spurious intr]$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ %x\n", status);
      __protocol->busy = FALSE;
      __i2c_stop ();
      // Clear interrupt
      *I2C_CONCLR (0) = CONCLR_SIC;
      return;
    }

  assert (__protocol->current < __protocol->n_command);
  i2c_status ();
  struct i2c_state *state = __protocol->state + __protocol->current;
  assert (state->func);
  DPRINTF ("func=%x\n", state->func);

  if (state->func (__protocol, status))
    {
      if (__protocol->current == __protocol->n_command)
	{
	  DPRINTF ("SUCCESS\n");
	  __protocol->success = TRUE;
	  __protocol->busy = FALSE;
	}
    }
  else
    {
      // Failed.
      DPRINTF ("command failed %x\n", status);
      iprintf ("command failed %x\n", status);
      __protocol->busy = FALSE;
      __i2c_stop ();
    }

  // Clear interrupt
  *I2C_CONCLR (0) = CONCLR_SIC;
}


