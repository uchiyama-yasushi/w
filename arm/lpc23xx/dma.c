
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

// GPDMA

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>

// Local scratch buffer. 1st 512B of USB RAM.
uint32_t dma_scratch_addr;

void
dma_init ()
{
  // Power on
  mcu_peripheral_power (PCONP_PCGPDMA, TRUE);

  // Start GPDMA (each channels are disabled yet)
  *DMACConfig = DMACConfigEnable; // Little-endian
  //  *DMACConfig = 0;
  dma_scratch_addr = USB_RAM_START;
}

void
dma_fini ()
{
  uint32_t dmac_base[] = { DMAC0_BASE, DMAC1_BASE };
  size_t i;

  // Disable each channel.
  for (i = 0; i < sizeof dmac_base / dmac_base[0]; i++)
    {
      volatile uint32_t *config = (volatile uint32_t *)
	(dmac_base[i] + DMA_CONFIG);
      // Furrther DMA requests to be ignored.
      *config |= ConfigHalt;
      // Wait current transfer completed.
      while (*config & ConfigActive)
	;
      // Disable channel.
      *config &= ~ChannelEnable;
    }
  // Stop GPDMA
  *DMACConfig = 0;
  mcu_peripheral_power (PCONP_PCGPDMA, FALSE);
}

bool
dma_address_check (uint32_t addr)
{
  // Check address is DMA accessible address or not.
  if (addr < USB_RAM_START || addr >= (USB_RAM_START + USB_RAM_SIZE))
    {
      //      printf ("%x not DMA accessible.\n", addr);
      return FALSE;
    }

  return TRUE;
}
