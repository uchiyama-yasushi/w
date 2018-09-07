
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

#ifndef _LPC23XX_DMA_H_
#define	_LPC23XX_DMA_H_
// GPDMA accessible memory
// 0x7fd00000-0x7fd03fff	USB RAM (16KB)
// 0x80000000-0x8000ffff	External SRAM bank 0
// 0x10000000-0x8100ffff	External SRAM bank 1
//
// * Can't access on-chip RAM (0x40000000-0x4000ffff

/*
   General Registers
*/
// Interrupt Status Register
#define	DMACIntStatus		((volatile uint32_t *)0xffe04000)	//RO
#define	 IntStatus0	0x01	// Status of channel 0 interrupts after masking
#define	 IntStatus1	0x02

// Interrupt Terminal Count Status Register
#define	DMACIntTCStatus		((volatile uint32_t *)0xffe04004)	//RO
#define	 IntTCStatus0	0x01	// Terminal count interrupt request status.
#define	 IntTCStatus1	0x02

// Interupt Terminal Count Clear Register
#define	DMACIntTCClear		((volatile uint32_t *)0xffe04008)	//WO
#define	 IntTCClear0	0x01	// Writing 1 clears terminal count interrupt.
#define	 IntTCClear1	0x02

// Interrupt Error Status Register
#define	DMACIntErrorStatus	((volatile uint32_t *)0xffe0400c)	//RO
#define	 IntErrorStatus0	0x01
#define	 IntErrorStatus1	0x02

// Interrupt Error Clear Register
#define	DMACIntErrClr		((volatile uint32_t *)0xffe04010)	//WO
#define	 IntErrClr0	0x01	// Writing 1 clears the interrupt error status.
#define	 IntErrClr1	0x02

// Raw Interrupt Terminal Count Status Register
#define	DMACRawIntTCStatus	((volatile uint32_t *)0xffe04014)	//RO
// Status of the terminal count interrupt for channle 0 prior to masking.
#define	 RawIntTCStatus0	0x01
#define	 RawIntTCStatus1	0x01

// Raw Error Interrupt Status Register
#define	DMACRawIntErrorStatus	((volatile uint32_t *)0xffe04018)	//RO
// Status of the error interrupt for channel 0 prior to masking.
#define	 RawIntErrorStatus0	0x01
#define	 RawIntErrorStatus1	0x02

// Enabled Channel Register
#define	DMACEnbldChns		((volatile uint32_t *)0xffe0401c)	//RO
#define	 EnabledChannels0	0x01
#define	 EnabledChannels1	0x02

// Software Burst Request Register
#define	DMACSoftBReq		((volatile uint32_t *)0xffe04020)
#define	 SoftBReqSSP0Tx		0x01
#define	 SoftBReqSSP0Rx		0x02
#define	 SoftBReqSSP1Tx		0x04
#define	 SoftBReqSSP1Rx		0x08
#define	 SoftBReqSDMMC		0x10
#define	 SoftBReqI2S0		0x20
#define	 SoftBReqI2S1		0x40

// Software Single Request Register
#define	DMACSoftSReq		((volatile uint32_t *)0xffe04024)
#define	 SoftReqSSP0Tx		0x01
#define	 SoftReqSSP0Rx		0x02
#define	 SoftReqSSP1Tx		0x04
#define	 SoftReqSSP1Rx		0x08
#define	 SoftReqSDMMC		0x10

// Software Last Burst Request Register
#define	DMACSoftLBReq		((volatile uint32_t *)0xffe04028)
#define	 SoftLBReqSDMMC		0x10

// Software Last Single Request Resgister
#define	DMACSoftLSReq		((volatile uint32_t *)0xffe0402c)
#define	 SoftLSReqSDMMC		0x10

// Configuration Register
#define	DMACConfig		((volatile uint32_t *)0xffe04030)
#define	 DMACConfigEnable	0x01
#define	 DMACConfigEndian	0x02	// 1 for Big-endian.

// Synchronization Register
#define	DMACSync		((volatile uint32_t *)0xffe04034)
#define	 DMACSyncMask		0xffff

/*
   Channel 0 Registers.
*/
#define	DMAC0_BASE		0xffe04100
#define	DMAC1_BASE		0xffe04120
#define	DMA_SRCADDR		0x00
#define	DMA_DSTADDR		0x04
#define	DMA_LLI			0x08
#define	DMA_CONTROL		0x0c
#define	DMA_CONFIG		0x10

//  Source Address Register
#define	DMACC0SrcAddr		((volatile uint32_t *)0xffe04100)
//  Destination Address Register
#define	DMACC0DstAddr		((volatile uint32_t *)0xffe04104)
//  Linked List Item Register
#define	DMACC0LLI		((volatile uint32_t *)0xffe04108)
#define	 LLI_MASK		0xfffffffc
//  Control Register
#define	DMACC0Control		((volatile uint32_t *)0xffe0410c)
#define	 TransferSizeMask	0xfff
#define	 TransferSizeShift	0
#define	 SBSizeMask		0x7
#define	 SBSizeShift		12
#define	 DBSizeMask		0x7
#define	 DBSizeShift		15
#define	  BurstSize1		0x0
#define	  BurstSize4		0x1
#define	  BurstSize8		0x2
#define	  BurstSize16		0x3
#define	  BurstSize32		0x4
#define	  BurstSize64		0x5
#define	  BurstSize128		0x6
#define	  BurstSize256		0x7
#define	 SWidthMask		0x7
#define	 SWidthShift		18
#define	 DWidthMask		0x7
#define	 DWidthShift		21
#define	  WidthSize8bit		0x0
#define	  WidthSize16bit	0x1
#define	  WidthSize32bit	0x2
#define	 DMASrcIncr		0x04000000
#define	 DMADstIncr		0x08000000
#define	 DMAProtMask		0x7
#define	 DMAProtShift		28
#define	  DMAProtPrivilege	0x1	// Default user
#define	  DMAProtBufferable	0x2	// Default Not bufferable.
#define	  DMAProtCacheable	0x4	// Default Not cacheable.
#define	 TerminalCountIntr	0x80000000

//  Configuration Register (Bit[17] is read-only)
#define	DMACC0Config		((volatile uint32_t *)0xffe04110)
#define	 ConfigReservedMask	0xfff80420	// Reserved bits.

#define	 ChannelEnable		0x00000001
#define	 SrcPeripheralMask	0xf
#define	 SrcPeripheralShift	1
#define	 DstPeripheralMask	0xf
#define	 DstPeripheralShift	6
#define	  PeripheralSSP0Tx	0
#define	  PeripheralSSP0Rx	1
#define	  PeripheralSSP1Tx	2
#define	  PeripheralSSP1Rx	3
#define	  PeripheralSDMMC	4
#define	  PeripheralI2S0	5
#define	  PeripheralI2S1	6

#define	 FlowCntrlMask		0x7
#define	 FlowCntrlShift		11
#define	  FlowCntrlM2M_DMA	0
#define	  FlowCntrlM2P_DMA	1
#define	  FlowCntrlP2M_DMA	2
#define	  FlowCntrlSP2DP_DMA	3
#define	  FlowCntrlSP2DP_DP	4
#define	  FlowCntrlM2P_P	5
#define	  FlowCntrlP2M_P	6
#define	  FlowCntrlSP2DP_SP	7
// M.. Memory
// P.. Peripheral
// DP. Destination Peripheral
// SP. Source Peripheral.
#define	 ConfigIntrError		0x00004000
#define	 ConfigIntrTerminalCount	0x00008000
#define	 ConfigLock			0x00010000
#define	 ConfigActive			0x00020000
#define	 ConfigHalt			0x00040000

/*
  Channel 1 Registers.
*/
//  Source Address Register
#define	DMACC1SrcAddr		((volatile uint32_t *)0xffe04120)
//  Destination Address Register
#define	DMACC1DstAddr		((volatile uint32_t *)0xffe04124)
//  Linked List Item Register
#define	DMACC1LLI		((volatile uint32_t *)0xffe04128)
//  Control Register
#define	DMACC1Control		((volatile uint32_t *)0xffe0412c)
//  Configuration Register (Bit[17] is read-only)
#define	DMACC1Config		((volatile uint32_t *)0xffe04130)

__BEGIN_DECLS
void dma_init (void);
void dma_fini (void);
bool dma_address_check (uint32_t);
#define	DMA_LOCAL_SCRATCH_SIZE	512
extern uint32_t dma_scratch_addr;
__END_DECLS

#endif
