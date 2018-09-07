
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

// Multimedia Card Interface DMA utils.

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>
#include <string.h>
#include <mci_dma.h>
#include <sys/timer.h>

#define	FS_SUPERBLOCK_SIZE	1024
#define	FS_DIRENT_SIZE		512
#define	WRITE_BUFFER_SIZE	512

STATIC thread_t __dma_sleeping_thread;
STATIC uint32_t __dma_error;
STATIC bool __dma_copy;
STATIC uint32_t __dma_addr;
STATIC cpu_status_t __dma_s;

STATIC void mci_dma_kick (bool);
STATIC bool mci_dma_sync (bool);

void
mci_dma_alloc (struct mci_dma *dma)
{
  uint8_t *p = (uint8_t *)USB_RAM_START;
  int i;

  p += DMA_LOCAL_SCRATCH_SIZE;	// Skip DMA local scratch buffer.
  dma->fs_control_block_size = FS_SUPERBLOCK_SIZE + FS_DIRENT_SIZE;
  dma->write_buffer_size = WRITE_BUFFER_SIZE;
  dma->read_buffer_size = USB_RAM_SIZE - (DMA_LOCAL_SCRATCH_SIZE +
					  FS_SUPERBLOCK_SIZE + FS_DIRENT_SIZE +
					  WRITE_BUFFER_SIZE * WRITE_BUFFER_BANK);

  dma->fs_control_block = p;
  p += dma->fs_control_block_size;

  // Write buffer.
  for (i = 0; i < WRITE_BUFFER_BANK; i++, p += dma->write_buffer_size)
    {
      iprintf ("DMA write buffer[%d]: %x\n", i, p);
      dma->write_buffer[i] = p;
    }
  dma->write_buffer_bank = 0;

  // Read buffer.
  dma->read_buffer = p;
}

void
mci_dmac0_write_start (uint32_t src_addr, bool polling)
{

  // If src_addr is not accessible from DMA, copy to DMA scratch buffer.
  if (!dma_address_check (src_addr))
    {
      memcpy ((uint8_t *)dma_scratch_addr, (uint8_t *)src_addr, 512);
      src_addr = dma_scratch_addr;
    }

  // Clear pending interrupt.
  *DMACIntTCClear = IntTCClear0;
  *DMACIntErrClr = IntErrClr0;
  // Source address.
  *DMACC0SrcAddr = src_addr;
  // Destination address.
  *DMACC0DstAddr = MCI_FIFO_ADDR;
  // not Linked List Item.
  *DMACC0LLI = 0;

  // Disable and reset configuration parameter of channel 0.
  *DMACC0Config &= ConfigReservedMask;

  *DMACC0Control = (512 & TransferSizeMask) |
    (BurstSize32 << SBSizeShift) |
    (BurstSize8 << DBSizeShift) |
    (WidthSize32bit << SWidthShift) |
    (WidthSize32bit << DWidthShift) |
    DMASrcIncr;

  *DMACC0Config |= (PeripheralSDMMC << DstPeripheralShift) |
    (FlowCntrlM2P_P << FlowCntrlShift);// Peripheral Flow Control.

  mci_dma_kick (polling);
}

bool
mci_dmac0_write_sync (bool polling)
{

  return mci_dma_sync (polling);
}

void
mci_dmac0_read_start (uint32_t dst_addr, bool polling)
{
  // Disable and reset configuration parameter of channel 0.
  *DMACC0Config &= ConfigReservedMask;

  // Clear pending interrupt.
  *DMACIntTCClear = IntTCClear0;
  *DMACIntErrClr = IntErrClr0;
  // Source address.
  *DMACC0SrcAddr = MCI_FIFO_ADDR;
  // Destination address.
  if (dma_address_check (dst_addr))
    {
      *DMACC0DstAddr = dst_addr;
      __dma_copy = FALSE;
    }
  else
    {
      // Requested address is not DMA accessible. use scratch buffer.
      *DMACC0DstAddr = dma_scratch_addr;
      __dma_addr = dst_addr;
      __dma_copy = TRUE;
    }

  // not Linked List Item.
  *DMACC0LLI = 0;

  *DMACC0Control = (512 & TransferSizeMask) |
    (BurstSize8 << SBSizeShift) |
    (BurstSize32 << DBSizeShift) |
    (WidthSize32bit << SWidthShift) |
    (WidthSize32bit << DWidthShift) |
    DMADstIncr;

  *DMACC0Config |= (PeripheralSDMMC << SrcPeripheralShift) |
    (FlowCntrlP2M_P << FlowCntrlShift);// Peripheral Flow Control.

  mci_dma_kick (polling);
}

bool
mci_dmac0_read_sync (bool polling)
{

  if (!mci_dma_sync (polling))
    {
      return FALSE;
    }

  if (__dma_copy)
    {
      memcpy ((uint8_t *)__dma_addr, (uint8_t *)dma_scratch_addr, 512);
    }

  return TRUE;
}

void
gpdma_intr ()
{

  if ((__dma_error = *DMACIntErrorStatus ) != 0)
    {
      // Clear Error interrupt.
      *DMACIntErrClr = __dma_error;
    }
  *DMACIntTCClear = *DMACIntTCStatus;
  *VICIntEnable &= ~VIC_GPDMA;
  thread_wakeup (__dma_sleeping_thread);
}

void
mci_dma_kick (bool polling)
{
  __dma_error = 0;

  if (!polling)
    {
      *VICIntSelect &= ~VIC_GPDMA; // IRQ
      *VICIntEnable |= VIC_GPDMA;
      __dma_sleeping_thread = current_thread;
      *DMACC0Control |= TerminalCountIntr;
      *DMACC0Config |= ConfigIntrTerminalCount | ConfigIntrError;
    }
  __dma_s = intr_suspend ();
  //  iprintf ("[R]src=%x dst=%x\n", *DMACC0SrcAddr, *DMACC0DstAddr);
  // Kick DMA. DMA channel is automatically enabled.
  *DMACC0Config |= ConfigLock;	// XXX required?
  // Start DMA
  *DMACC0Config |= ChannelEnable;
}

bool
mci_dma_sync (bool polling)
{

  if (polling)
    {
      // Wait complete or error.
      while (*DMACC0Config & ChannelEnable)	// XXX Timeout.
	;
      __dma_error = *DMACRawIntErrorStatus;
    }
  else
    {
      thread_block (NULL);
    }
  intr_resume (__dma_s);

  if (__dma_error)
    {
      iprintf ("DMA Error %x (P2M)\n", __dma_error);
      return FALSE;
    }

  return TRUE;
}
