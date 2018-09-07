
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

#ifndef _LPC23XX_MCI_H_
#define	_LPC23XX_MCI_H_
// Power Control Register
#define	MCI_POWER	((volatile uint32_t *)0xe008c000)
#define	 POWER_OFF		0x00
#define	 POWER_UP		0x02
#define	 POWER_ON		0x03
#define	 POWER_OPENDRAIN	0x40
#define	 POWER_ROD		0x80

// Clock Control Register
#define	MCI_CLOCK	((volatile uint32_t *)0xe008c004)
#define	 CLOCK_DIV_MASK		0xff
#define	 CLOCK_DIV_SHIFT	0
#define	 CLOCK_ENABLE		0x100
#define	 CLOCK_PWRSAVE		0x200
#define	 CLOCK_BYPASS		0x400
#define	 CLOCK_WIDEBUS		0x800	// MCIDAT0 or MCIDAT3:0

// Argument Register
#define	MCI_ARGUMENT	((volatile uint32_t *)0xe008c008)

// Command Register	(CPSM)
#define	MCI_COMMAND	((volatile uint32_t *)0xe008c00c)
#define	 COMMAND_INDEX_MASK	0x3f
#define	 COMMAND_INDEX_SHIFT	0
#define	 COMMAND_RESPONSE	0x040	// 48bit short response
#define	 COMMAND_LONGRSP	0x080	// 136bit long response
#define	 COMMAND_INTERRUPT	0x100
#define	 COMMAND_PENDING	0x200
#define	 COMMAND_ENABLE		0x400

// Command Response Register
#define	MCI_RESPCMD	((volatile uint32_t *)0xe008c010)	//RO
#define	 RESPCMD_MASK		0x3f
#define	 RESPCMD_SHIFT		0

// Response Registers.
#define	MCI_RESPONSE0	((volatile uint32_t *)0xe008c014)	//RO
#define	MCI_RESPONSE1	((volatile uint32_t *)0xe008c018)	//RO
#define	MCI_RESPONSE2	((volatile uint32_t *)0xe008c01c)	//RO
#define	MCI_RESPONSE3	((volatile uint32_t *)0xe008c020)	//RO

// Data Timer Register (timeout)
#define	MCI_DATATIMER	((volatile uint32_t *)0xe008c024)

// Data Length Register (# of data bytes to be trasferred)
#define	MCI_DATALENGTH	((volatile uint32_t *)0xe008c028)
#define	 DATALENGTH_MASK	0xffff
#define	 DATALENGTH_SHIFT	0

// Data Control Register	(DPSM)
#define	MCI_DATACTRL	((volatile uint32_t *)0xe008c02c)
#define	 DATACTRL_ENABLE	0x01 //Data trasfer enable.
#define	 DATACTRL_DIRECTION	0x02 //0: controller->card, 1: card->controller
#define	 DATACTRL_MODE		0x04 //0: block data , 1: stream data
#define	 DATACTRL_DMAENABLE	0x08
#define	 DATACTRL_BLOCKSIZE_MASK	0x0f
#define	 DATACTRL_BLOCKSIZE_SHIFT	4

// Data Counter Register
#define	MCI_DATACNT	((volatile uint32_t *)0xe008c030)	//RO
#define	 DATACNT_MASK		0xffff
#define	 DATACNT_SHIFT		0

// Status Register
#define	MCI_STATUS	((volatile uint32_t *)0xe008c034)	//RO
#define	 STATUS_CMDCRCFAIL		0x000001
#define	 STATUS_DATACRCFAIL		0x000002
#define	 STATUS_CMDTIMEOUT		0x000004
#define	 STATUS_DATATIMEOUT		0x000008
#define	 STATUS_TXUNDERRUN		0x000010
#define	 STATUS_RXOVERRUN		0x000020
#define	 STATUS_CMDRESPEND		0x000040
#define	 STATUS_CMDSENT			0x000080
#define	 STATUS_DATAEND			0x000100
#define	 STATUS_STARTBITERR		0x000200
#define	 STATUS_DATABLOCKEND		0x000400
#define	 STATUS_CMDACTIVE		0x000800
#define	 STATUS_TXACTIVE		0x001000
#define	 STATUS_RXACTIVE		0x002000
#define	 STATUS_TXFIFOHALFEMPTY		0x004000
#define	 STATUS_RXFIFOHALFFULL		0x008000
#define	 STATUS_TXFIFOFULL		0x010000
#define	 STATUS_RXFIFOFULL		0x020000
#define	 STATUS_TXFIFOEMPTY		0x040000
#define	 STATUS_RXFIFOEMPTY		0x080000
#define	 STATUS_TXDATAAVLBL		0x100000
#define	 STATUS_RXDATAAVLBL		0x200000
#define	 STATUS_MASK			0x3fffff

// Clear Register
#define	MCI_CLEAR	((volatile uint32_t *)0xe008c038)	//WO
#define	 CLEAR_CMDCRCFAILCLR		0x001
#define	 CLEAR_DATACRCFAILCLR		0x002
#define	 CLEAR_CMDTIMEOUTCLR		0x004
#define	 CLEAR_DATTIMEOUTCLR		0x008
#define	 CLEAR_TXUNDERRUNCLR		0x010
#define	 CLEAR_RXOVERRUNCLR		0x020
#define	 CLEAR_CMDRESPENDCLR		0x040
#define	 CLEAR_CMDSENTCLR		0x080
#define	 CLEAR_DATAENDCLR		0x100
#define	 CLEAR_STARTBITERRCLR		0x200
#define	 CLEAR_DATABLOCKENDCLR		0x400
#define	 CLEAR_ALL			0x7ff

// Interrupt Mask Register
#define	MCI_MASK0	((volatile uint32_t *)0xe008c03c)

// FIFO Counter Register
#define	MCI_FIFOCNT	((volatile uint32_t *)0xe008c048)	//RO
#define	 FIFOCNT_MASK	0x7fff
#define	 FIFOCNT_SHIFT	0

// Data FIFO Register
#define	MCI_FIFO_ADDR	0xe008c080
#define	MCI_FIFO_START	((volatile uint32_t *)MCI_FIFO_ADDR)
#define	MCI_FIFO_END	((volatile uint32_t *)(MCI_FIFO_ADDR + 60))

//----------------------------------------------------------------------
// Software define
enum mci_bus_width
  {
    SD_DATA_1BIT,
    SD_DATA_4BIT,
  };

__BEGIN_DECLS

struct card_info
{
  uint32_t rca;
  enum card_type
    {
      CARD_MMC,
      CARD_SD,
      CARD_SDHC,
    }
    type;
  bool block_address;
};

struct mci_controller
{
  enum sdcard_bus_width
    {
      SDCARD_WIDTH_1BIT,
      SDCARD_WIDTH_4BIT,
    }
    bus_width;
  bool use_dma;
  bool polling;
  bool slow_clock;
};

struct mci_context
{
  struct card_info card;
  struct mci_controller controller;
  daddr_t offset;
  bool debug;
};

bool mci_init (struct mci_context *);
bool mci_card_init (struct mci_context *);

struct block_io_ops;
int mmc_init (struct block_io_ops *);

bool mmc_drive (void *, uint8_t);
bool mmc_read (void *, uint8_t *, daddr_t);
bool mmc_read_n (void *, uint8_t *, daddr_t, int);
bool mmc_write (void *, uint8_t *, daddr_t);
bool mmc_write_n (void *, uint8_t *, daddr_t, int);
__END_DECLS
#endif
