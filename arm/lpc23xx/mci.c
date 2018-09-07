
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

// SD Card Interface.

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>
#include <sys/delay.h>
#include <sys/timer.h>
#include <mci_dma.h>

STATIC int mci_debug __attribute__((unused));
//#define	MCI_DEBUG
#ifdef MCI_DEBUG
#define	DPRINTF(fmt, args...)	if (mci_debug) iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif
#ifdef MMC_VERBOSE
#define	LPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	LPRINTF(fmt, args...)	((void)0)
#endif

struct cpsm_command;

STATIC void mci_power (struct mci_context *, bool);
STATIC bool mci_tran_state (struct mci_context *);
STATIC bool mci_cpsm_idle (void);
STATIC bool mci_cpsm_command (struct card_info *, struct cpsm_command *);
#ifdef MCI_DEBUG
STATIC void mci_status_print (uint32_t);
#endif

STATIC uint32_t response[4];

STATIC bool match_status_sent (uint32_t);
STATIC bool match_status_respond (uint32_t);
STATIC bool match_status_crcfailed (uint32_t);
STATIC bool match_response_none (uint32_t *);
STATIC bool match_response_initialize (uint32_t *);
STATIC bool match_response_tran (uint32_t *);
STATIC bool match_response_cmd8 (uint32_t *);
STATIC bool match_response_rw (uint32_t *);
STATIC bool match_response_4bitmode (uint32_t *);

STATIC struct cpsm_command
{
  int command;
  uint32_t arg;
  uint32_t *response_buf;
  int response_n;
  int timeout;	// msec.
#define	DEFAULT_TIMEOUT		1000	// 1sec.
  bool (*match_status) (uint32_t);
  bool (*match_response) (uint32_t *);
  bool rca_required;
} cpsm_command[] =
  {
#define	MMC_CMD0	0
    { 0, 0, NULL, 0, DEFAULT_TIMEOUT, match_status_sent, match_response_none, FALSE },
#define	MMC_CMD1	1
    { 1, 0x00ff8000, response, 1, 0, match_status_crcfailed, match_response_initialize, FALSE },
#define	MMC_CMD2	2
    { 2, 0, response, 4, DEFAULT_TIMEOUT, match_status_respond, match_response_none, FALSE },//R2
#define	MMC_CMD3	3
    { 3, 0, response, 1, DEFAULT_TIMEOUT, match_status_respond, match_response_none, FALSE },//R6
#define	MMC_CMD7	4
    { 7, 0, response, 1, DEFAULT_TIMEOUT, match_status_respond, match_response_none, TRUE },//RCA R1
#define	MMC_CMD8	5
    { 8, 0x1aa, response, 1, DEFAULT_TIMEOUT, match_status_respond, match_response_cmd8, FALSE },
#define	MMC_CMD9	6
    { 9, 0, response, 4, DEFAULT_TIMEOUT, match_status_respond, match_response_none, TRUE },//RCA R2
#define	MMC_CMD10	7
    { 10, 0, response, 4, DEFAULT_TIMEOUT, match_status_respond, match_response_none, TRUE },//RCA R2
#define	MMC_CMD13	8
    { 13, 0, response, 1, DEFAULT_TIMEOUT, match_status_respond, match_response_tran, TRUE },//RCA R1
#define	MMC_CMD17	9
    { 17, 0, response, 1, DEFAULT_TIMEOUT, match_status_respond, match_response_rw, FALSE }, //R1
#define	MMC_CMD24	10
    { 24, 0, response, 1, DEFAULT_TIMEOUT, match_status_respond, match_response_rw, FALSE }, //R1
#define	MMC_CMD55	11
    { 55, 0, response, 1, DEFAULT_TIMEOUT, match_status_respond, match_response_none, FALSE },//R1
#define	MMC_ACMD41	12
    { 41, 0, response, 1, DEFAULT_TIMEOUT, match_status_crcfailed, match_response_initialize, FALSE },//R3
#define	MMC_ACMD6	13
    { 6,  2, response, 1, DEFAULT_TIMEOUT, match_status_respond, match_response_4bitmode, FALSE },
  };

static uint32_t __msec;

//----------------------------------------------------------------------
// Timeout utils.
//----------------------------------------------------------------------

void
md_timer_counter_set (uint32_t msec)
{
  __msec = msec;
  *T2TC = 0;
  md_timer_start (2, msec, COUNTER_MSEC, COUNTER_STOP);
  //  iprintf ("%s:%d %d\n", __FUNCTION__, __msec, *T2TC);
}

uint32_t
md_timer_counter ()
{
  uint32_t t = __msec - *T2TC;
  //  iprintf ("%s:%d %d\n", __FUNCTION__, __msec, *T2TC);

  return (int32_t)t < 0 ? 0 : t;
}

//----------------------------------------------------------------------
// LPC2388 setting.
//----------------------------------------------------------------------
bool
mci_init (struct mci_context *ctx)
{
  mci_debug = ctx->debug;

  // Pins
  mcu_pin_select2 (0, 19, PIN_FUNC2);	// MCICLK
  mcu_pin_mode (0, 19, PIN_HIZ);
  mcu_pin_select2 (0, 20, PIN_FUNC2);	// MCICMD
  mcu_pin_mode (0, 20, PIN_HIZ);
  mcu_pin_select2 (0, 21, PIN_FUNC2);	// MCIPWR
  mcu_pin_mode (0, 21, PIN_HIZ);
  mcu_pin_select2 (0, 22, PIN_FUNC2);	// MCIDAT0
  mcu_pin_mode (0, 22, PIN_HIZ);
  if (ctx->controller.bus_width == SDCARD_WIDTH_4BIT)
    {
      mcu_pin_select2 (2, 11, PIN_FUNC2);	// MCIDAT1
      mcu_pin_mode (2, 11, PIN_HIZ);
      mcu_pin_select2 (2, 12, PIN_FUNC2);	// MCIDAT2
      mcu_pin_mode (2, 12, PIN_HIZ);
      mcu_pin_select2 (2, 13, PIN_FUNC2);	// MCIDAT3
      mcu_pin_mode (2, 13, PIN_HIZ);
    }
  mdelay (10);
  // Power on
  mcu_peripheral_power (PCONP_PCSDC, TRUE);
  // Clock on
  mdelay (10);

  if (ctx->controller.slow_clock)
    {
      mcu_peripheral_clock (PCLK_MCI, CCLK8);	//72/8=9MHz
    }
  else
    {
      mcu_peripheral_clock (PCLK_MCI, CCLK4);	//72/4=18MHz
    }
  mdelay (100);
  printf ("PWR %x CLK %x PIN %x\n", *PCONP, *PCLKSEL1, *PINSEL1);

  return TRUE;
}

void
mci_power (struct mci_context *ctx, bool on)
{
  if (on)
    {
      // Power up
      *MCI_POWER = POWER_UP;
      // Wait until power supply is stable.
      mdelay (10);
      *MCI_POWER = POWER_ON;
      // Set clock. while identification mode, MCICLK must be less than 400kHz.
      // MCICLK = MCLK / (2 * (ClkDiv + 1))
      // ClkDiv = MCLK/(2 * MCICLK) -1; MCICLK=400kHz, MCLK=18MHz, ClkDiv = 21.5
      // ClkDiv = MCLK/(2 * MCICLK) -1; MCICLK=400kHz, MCLK=9MHz, ClkDiv = 10.25
      // ClkDiv = 0xff, MCLK=9MHz -> 17.6kHz
      if (ctx->controller.slow_clock)
	{
	  *MCI_CLOCK = 0xff | CLOCK_ENABLE | CLOCK_PWRSAVE;
	}
      else
	{
	  *MCI_CLOCK = 22 | CLOCK_ENABLE | CLOCK_PWRSAVE;
	}
    }
  else
    {
      // Reset register.
      *MCI_CLOCK = 0;
      *MCI_POWER = 0;
      *MCI_MASK0 = 0;
      *MCI_COMMAND = 0;
      *MCI_DATACTRL = 0;
      *MCI_CLEAR = CLEAR_ALL;	// Clear all status.
    }
  mdelay (10);
}

bool
mci_card_init (struct mci_context *ctx)
{
  int i;

  mci_power (ctx, FALSE);
  mci_power (ctx, TRUE);

  // [CMD0] Reset
  mci_cpsm_command (NULL, &cpsm_command[MMC_CMD0]);

  // [CMD1] MMC card response this. SD card don't response.
  if (mci_cpsm_command (NULL, &cpsm_command[MMC_CMD1]))
    {
      ctx->card.type = CARD_MMC;
      return FALSE;	// not supported.
    }

  // [CMD0] Reset again for SD, SDHC card.
  mci_cpsm_command (NULL, &cpsm_command[MMC_CMD0]);

  // [CMD8] Check SD card or SDHC card.
  if (mci_cpsm_command (NULL, &cpsm_command[MMC_CMD8]))
    {
      printf ("SDHC card\n");
      ctx->card.type = CARD_SDHC;
    }
  else
    {
      printf ("SD card\n");
      ctx->card.type = CARD_SD;
    }

  // [CMD41] Initialize SD, SDHC card.
  cpsm_command[MMC_ACMD41].arg =
    ctx->card.type == CARD_SDHC ? 0x40ff8000: 0x00ff8000;

  for (i = 0; i < 1000; i++)
    {
      if (mci_cpsm_command (NULL, &cpsm_command[MMC_CMD55]) &&
	  mci_cpsm_command (NULL, &cpsm_command[MMC_ACMD41]))
	break;
    }
  if (i == 1000)
    {
      printf ("ACMD41 failed.\n");
      return FALSE;
    }

  // Check OCR
  ctx->card.block_address = FALSE;
  if (ctx->card.type == CARD_SDHC)
    {
      if (response[3] & 0x40000000)
	{
	  printf ("SDHC block addresssing.\n");
	  ctx->card.block_address = TRUE;
	}
    }

  // [CMD2] Get CID.
  if (!mci_cpsm_command (NULL, &cpsm_command[MMC_CMD2]))
    return FALSE;

  // [CMD3] Get RCA.
  if (!mci_cpsm_command (NULL, &cpsm_command[MMC_CMD3]))
    return FALSE;
  ctx->card.rca = response[0];
  printf ("RCA=%x\n", ctx->card.rca);

  // [CMD9] Get CSD
  if (!mci_cpsm_command (&ctx->card, &cpsm_command[MMC_CMD9]))
    return FALSE;

  // Full speed clock.
  if (ctx->controller.slow_clock)
    {
      *MCI_CLOCK |= CLOCK_BYPASS;
    }

  // [CMD10] Get CID again. (paranoia)
  if (!mci_cpsm_command (&ctx->card, &cpsm_command[MMC_CMD10]))
    return FALSE;

  // [CMD7] Set RCA to r/w
  if (!mci_cpsm_command (&ctx->card, &cpsm_command[MMC_CMD7]))
    return FALSE;

  int bus_width = 1;
  if (ctx->controller.bus_width == SDCARD_WIDTH_4BIT)
    {
      // Try wide bus.
      cpsm_command[MMC_CMD55].arg = ctx->card.rca;
      for (i = 0; i < 10; i++)
	if (mci_cpsm_command (&ctx->card, &cpsm_command[MMC_CMD55]) &&
	    mci_cpsm_command (&ctx->card, &cpsm_command[MMC_ACMD6]))
	  {
	    *MCI_CLOCK |= CLOCK_WIDEBUS;
	    bus_width = 4;
	  }
    }
  ctx->controller.bus_width =
    bus_width == 4 ? SDCARD_WIDTH_4BIT : SDCARD_WIDTH_1BIT;

  printf ("Bus width = %dbit.\n", bus_width);

  return TRUE;
}


bool
mci_cpsm_command (struct card_info *card, struct cpsm_command *cmd)
{
  uint32_t r;
#ifdef MCI_DEBUG
  uint32_t e;
  uint32_t old_r = 0;
#endif
  uint32_t command;
  uint32_t *buf = cmd->response_buf;
  int n = cmd->response_n;
  int i;

  md_timer_counter_set (cmd->timeout);
 retry:
  // Make sure to CPSM idle.
  if (!mci_cpsm_idle ())
    {
      DPRINTF ("don't idle state.\n");
      return FALSE;
    }

#ifdef MCI_DEBUG
  DPRINTF ("CMD%d %d", cmd->command, md_timer_counter ());
  if (mci_debug)
    mci_status_print (*MCI_STATUS);
#endif

  // If RCA command, set it.
  *MCI_ARGUMENT = cmd->rca_required ? card->rca : cmd->arg;
  command = cmd->command | COMMAND_ENABLE;

  if (n > 0)
    command |= COMMAND_RESPONSE;
  if (n > 1)
    command |= COMMAND_LONGRSP;

  // CPSM start.
  *MCI_COMMAND = command;
  // Wait command response.
#ifdef MCI_DEBUG
  if (mci_debug)
    mci_status_print (old_r = *MCI_STATUS);
#endif
  while (!cmd->match_status (r = *MCI_STATUS))
    {
#ifdef MCI_DEBUG
      if (mci_debug)
	{
	  if ((e = ((r ^ old_r) & r)))
	    {
	      mci_status_print (e);
	    }
	  old_r = r;
	}
#endif
      if (r & STATUS_CMDTIMEOUT)
	{
	  if (md_timer_counter () == 0)
	    {
	      DPRINTF ("timeout\n");
	      return FALSE;
	    }
	  goto retry;
	}
    }
  // Store response
  for (i = 0; i < n; i++)
    {
      buf[i] = *(MCI_RESPONSE0 + i);
    }
#ifdef MCI_DEBUG
  if (mci_debug)
    {
      printf ("response = ");
      for (i = 0; i < n; i++)
	{
	  printf ("%x ", buf[i]);
	}
      printf ("\n");
    }
  if (buf && !cmd->match_response (buf))
    {
      printf ("match response failed\n");
    }
#endif

  // Check response value.
  return buf ? cmd->match_response (buf) : TRUE;
}

bool
mci_cpsm_idle ()
{
  uint32_t r;

  // Stop CPSM
  *MCI_COMMAND = 0;

  while (((r = *MCI_STATUS) & STATUS_CMDACTIVE) && md_timer_counter () != 0)
    ;
  if (md_timer_counter () == 0)
    return FALSE;

  *MCI_CLEAR = *MCI_STATUS & CLEAR_ALL;

  return TRUE;
}

bool
mci_tran_state (struct mci_context *ctx)
{

  // [CMD13] Get status.
  return mci_cpsm_command (&ctx->card, &cpsm_command[MMC_CMD13]);
}

bool
match_response_none (uint32_t *buf __attribute__((unused)))
{

  return TRUE;
}

bool
match_response_initialize (uint32_t *buf)
{

  return (buf[0] & 0x80000000) != 0;
}

bool
match_response_cmd8 (uint32_t *buf)
{

  return buf[0] == 0x1aa;
}

bool
match_response_tran (uint32_t *buf)
{

  return (buf[0] & 0x1e00) == 0x800;
}

bool
match_response_rw (uint32_t *buf)
{

  return (buf[0] & 0xc0580000) == 0;
}

bool
match_response_4bitmode (uint32_t *buf)
{

  return (buf[0] & 0xfdf90000) == 0;
}

// CMD0 (reset)
bool
match_status_sent (uint32_t r)
{

  return r & STATUS_CMDSENT;
}

// CMD8,55,2,3,9,10,7,13,17,24
bool
match_status_respond (uint32_t r)
{

  return r & STATUS_CMDRESPEND;
}

// CMD1,12,41 (intialize)
bool
match_status_crcfailed (uint32_t r)
{

  return r & STATUS_CMDCRCFAIL;
}

bool
mmc_read (void *self, uint8_t *buf, daddr_t lba)
{
  struct mci_context *ctx = (struct mci_context *)self;
  struct cpsm_command *cmd = cpsm_command + MMC_CMD17;

  mci_tran_state (ctx);

  *MCI_DATATIMER = ~0;
  *MCI_DATALENGTH = 512;

  *MCI_DATACTRL = DATACTRL_ENABLE | DATACTRL_DIRECTION |
    ((9  & DATACTRL_BLOCKSIZE_MASK) << DATACTRL_BLOCKSIZE_SHIFT);

  if (ctx->controller.use_dma)
    *MCI_DATACTRL |= DATACTRL_DMAENABLE;

  *MCI_CLEAR = CLEAR_ALL;

  lba += ctx->offset;
  cmd->arg = ctx->card.block_address ? lba : lba * 512;

  if (!mci_cpsm_command (&ctx->card, cmd))
    return FALSE;

  if (ctx->controller.use_dma)
    {
      mci_dmac0_read_start ((uint32_t)buf, ctx->controller.polling);
      if (!mci_dmac0_read_sync (ctx->controller.polling))
	{
	  *MCI_DATACTRL = 0;  // DMA failed. Stop DPSM
	  return FALSE;
	}
      md_timer_counter_set (cmd->timeout);
      while (*MCI_STATUS & STATUS_RXACTIVE &&
	     md_timer_counter () != 0)
	;
    }
  else
    {
      // PIO (for debug)
      uint32_t *fifobuf = (uint32_t *)buf;
      int i, j;
      for (i = 0; i < 128; i += 8)
	{
	  md_timer_counter_set (cmd->timeout);
	  while ((*MCI_STATUS & STATUS_RXFIFOHALFFULL) == 0 &&
		 md_timer_counter () != 0)
	    ;
	  for (j = 0; j < 8; j++)
	    {
	      fifobuf[i + j] = *MCI_FIFO_START;
	    }
	}
    }
  // Stop DPSM
  *MCI_DATACTRL = 0;

  if (md_timer_counter () == 0)
    {
#ifdef MCI_DEBUG
      printf ("timeout\n");
      mci_status_print (*MCI_STATUS);
#endif
      return FALSE;
    }

  return TRUE;
}

bool
mmc_write (void *self, uint8_t *buf, daddr_t lba)
{
  struct mci_context *ctx = (struct mci_context *)self;
  struct cpsm_command *cmd = cpsm_command + MMC_CMD24;

  mci_tran_state (ctx);

  *MCI_DATATIMER = ~0;
  *MCI_DATALENGTH = 512;

  *MCI_DATACTRL = DATACTRL_ENABLE |
    ((9  & DATACTRL_BLOCKSIZE_MASK) << DATACTRL_BLOCKSIZE_SHIFT);
  if (ctx->controller.use_dma)
    *MCI_DATACTRL |= DATACTRL_DMAENABLE;

  *MCI_CLEAR = CLEAR_ALL;

  lba += ctx->offset;
  cmd->arg = ctx->card.block_address ? lba : lba * 512;

  if (!mci_cpsm_command (&ctx->card, cmd))
    {
      printf ("CMD24 failed.\n");
      return FALSE;
    }
  if (ctx->controller.use_dma)
    {
      mci_dmac0_write_start ((uint32_t)buf, ctx->controller.polling);
      if (!mci_dmac0_write_sync (ctx->controller.polling))
	{
	  *MCI_DATACTRL = 0;  // DMA failed. Stop DPSM
	  return FALSE;
	}
      md_timer_counter_set (cmd->timeout);
      while (*MCI_STATUS & STATUS_TXACTIVE &&
	     md_timer_counter () != 0)
	;
    }
  else
    {
      // PIO (for debug)
      uint32_t *fifobuf = (uint32_t *)buf;
      int i;
      md_timer_counter_set (cmd->timeout);
      while ((*MCI_STATUS & STATUS_CMDACTIVE) && md_timer_counter () != 0)
	;
      if (md_timer_counter () == 0)
	{
	  printf ("timeout 1\n");
	  *MCI_DATACTRL = 0;  // CPSM failed. Stop DPSM
	  return FALSE;
	}
      md_timer_counter_set (cmd->timeout);
      for (i = 0; i < 512 / 4; i++)
	{
	  while (*MCI_STATUS & STATUS_TXFIFOFULL && md_timer_counter () != 0)
	    ;
	  *MCI_FIFO_START = fifobuf[i];
	}
      // Transmit in prgress.
      while (*MCI_STATUS & STATUS_TXACTIVE &&
	     md_timer_counter () != 0)
	;
    }
  // Stop DPSM
  *MCI_DATACTRL = 0;

  if (md_timer_counter () == 0)
    {
#ifdef MCI_DEBUG
      printf ("timeout.\n");
      mci_status_print (*MCI_STATUS);
#endif
      return FALSE;
    }

  return TRUE;
}

#ifdef MCI_DEBUG
void
mci_status_print (uint32_t r)
{
  static struct {
    uint32_t bit;
    const char *message;
  } mci_error_message[] =
      {
	{ STATUS_CMDCRCFAIL, "CRC failed (command)" },
	{ STATUS_DATACRCFAIL, "CRC failed (data)" },
	{ STATUS_CMDTIMEOUT, "Timeout (command)" },
	{ STATUS_DATATIMEOUT, "Timeout (data)" },
	{ STATUS_TXUNDERRUN, "Tx FIFO Underrun" },
	{ STATUS_RXOVERRUN, "Rx FIFO Overrun" },
	{ STATUS_CMDRESPEND, "Response received." },
	{ STATUS_CMDSENT, "Command sent" },
	{ STATUS_DATAEND, "Data end" },
	{ STATUS_STARTBITERR, "Start bit error" },
	{ STATUS_DATABLOCKEND, "Data sent/received" },
	{ STATUS_CMDACTIVE, "Command transfer progress" },
	{ STATUS_TXACTIVE, "Data Tx progress" },
	{ STATUS_RXACTIVE, "Data Rx progress" },
	{ STATUS_TXFIFOHALFEMPTY, "Tx FIFO half empty" },
	{ STATUS_RXFIFOHALFFULL, "Rx FIFO half full" },
	{ STATUS_TXFIFOFULL, "Tx FIFO full" },
	{ STATUS_RXFIFOFULL, "Rx FIFO full" },
	{ STATUS_TXFIFOEMPTY, "Tx FIFO empty" },
	{ STATUS_RXFIFOEMPTY, "Rx FIFO empty" },
	{ STATUS_TXDATAAVLBL, "Tx FIFO data available." },
	{ STATUS_RXDATAAVLBL, "Rx FIFO data available." },
      };
  size_t i;

  assert ((r & ~STATUS_MASK) == 0);
  if (r == 0)
    {
      printf ("Idle\n");
      return;
    }

  for (i = 0; i < sizeof mci_error_message / (sizeof mci_error_message[0]); i++)
    {
      if (mci_error_message[i].bit & r)
	printf ("|%s", mci_error_message[i].message);
    }
  printf ("\n");
}
#endif
