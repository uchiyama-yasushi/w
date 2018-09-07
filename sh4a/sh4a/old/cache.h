
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

// SH4A: SH7785
#ifndef _SH7785_CACHE_H_
#define	_SH7785_CACHE_H_

#define	SH7785_ICACHE_SIZE	32768
#define	SH7785_DCACHE_SIZE	32768

#define	SH7785_CACHE_LINESZ	32
#define	SH7785_CACHE_WAY	4

// SH7785_DCACHE_SIZE / SH7785_CACHE_LINESZ / SH7785_CACHE_WAY = 256
#define	SH7785_CACHE_ENTRY	256

#define	ROUND_CACHELINE_32(x)	(((x) + 31) & ~31)
#define	TRUNC_CACHELINE_32(x)	((x) & ~31)

// Cache control register
#define	SH7785_CCR	((volatile uint32_t *)0xff00001c)
// Instruction Cache Invalidate (all entry)
#define	 CCR_ICI	0x800
// Instruction Cache Enable
#define	 CCR_ICE	0x100
// Operand Cache Invalidate (all entry)
#define	 CCR_OCI	0x8
// P1-area Copy Back mode (1:write-back, 0:write-thru)
#ifndef SH4A_EXT_ADDR32 // PMB->WT bit.
#define	 CCR_CB		0x4
#endif
// P0,U0,P3-area Write Thru (1:write-thru, 0:write-back)
#define	 CCR_WT		0x2
// Operand Cache Enable
#define	 CCR_OCE	0x1

#define	SH7785_QACR0	((volatile uint32_t *)0xff000038)
// When MMU disable, set physical address 28:26
#define	 QACR_AREA_MASK		0x7
#define	 QACR_AREA_SHIFT	2
#define	SH7785_QACR1	((volatile uint32_t *)0xff00003c)

// RAM Control Register
#define	SH7785_RAMCR	((volatile uint32_t *)0xff000074)
// Internal Memmory Access mode. (L-memory)
#define	 RAMCR_RMD	0x200
// Internal Memmory Protection. (L-memory)
#define	 RAMCR_RP	0x100
// I-cache 2way mode (default 4way)
#define	 RAMCR_IC2W	0x80
// D-cache 2way mode (default 4way)
#define	 RAMCR_OC2W	0x40
// I-cache way-prediction (SH7785)
#define	 RAMCR_ICWPD	0x20


//
// Memory-Maped Cache Configuratiion
//

// I-cache address array
#define	SH7785_CCIA	0xf0000000
// address-field
#define	 CCIA_A			0x8	// Associative bit.
#define	 CCIA_ENTRY_MASK	0xff
#define	 CCIA_ENTRY_SHIFT	5
#define	 CCIA_WAY_MASK		0x3
#define	 CCIA_WAY_SHIFT		13
// data-field (physical tag)
#define	 CCIA_V			0x1	// Valid bit
#define	 CCIA_TAG_MASK		0x3fffff
#define	 CCIA_TAG_SHIFT		10

// I-cache data array
#define	SH7785_CCID	0xf1000000
// address-filed
#define	 CCID_LINE_MASK		0x3	// line size is 32B
#define	 CCID_LINE_SHIFT	2
#define	 CCID_ENTRY_MASK	0xff	// 256 entry
#define	 CCID_ENTRY_SHIFT	5
#define	 CCID_WAY_MASK		0x3	// 4way
#define	 CCID_WAY_SHIFT		13
// data-filed (cached data)
//


// D-cache address array
#define	SH7785_CCDA	0xf4000000
// address-field
#define	 CCDA_A			0x8	// Associative bit.
#define	 CCDA_ENTRY_MASK	0xff
#define	 CCDA_ENTRY_SHIFT	5
#define	 CCDA_WAY_MASK		0x3
#define	 CCDA_WAY_SHIFT		13
// data-field (physical tag)
#define	 CCDA_V			0x1	// Valid bit
#define	 CCDA_U			0x2	// Dirty bit
#define	 CCDA_TAG_MASK		0x3fffff
#define	 CCDA_TAG_SHIFT		10

// D-cache data array
#define	SH7785_CCDD	0xf5000000
// address-filed
#define	 CCDD_LINE_MASK		0x3	// line size is 32B
#define	 CCDD_LINE_SHIFT	2
#define	 CCDD_ENTRY_MASK	0xff	// 256 entry
#define	 CCDD_ENTRY_SHIFT	5
#define	 CCDD_WAY_MASK		0x3	// 4way
#define	 CCDD_WAY_SHIFT		13
// data-field (cached data)
//

// Store Queue
#define	SH7785_SQ	0xe0000000
#define	 SQ_BANK	0x20
#define	 SQ_LW_MASK	0x7
#define	 SQ_LW_SHIFT	2

__BEGIN_DECLS
void sh7785_cache_enable (void);
void sh7785_cache_disable (void);

void sh7785_icache_sync_all (void);
void sh7785_icache_sync_range (vaddr_t, vsize_t);
void sh7785_icache_sync_range_index (vaddr_t, vsize_t);

void sh7785_dcache_wbinv_all (void);
void sh7785_dcache_wbinv_range (vaddr_t, vsize_t);
void sh7785_dcache_inv_range (vaddr_t, vsize_t);
void sh7785_dcache_wb_range (vaddr_t, vsize_t);
void sh7785_dcache_wbinv_range_index (vaddr_t, vsize_t);
#ifdef DEBUG
void sh7785_dcache_array_dump (vaddr_t, vsize_t);
#endif

__END_DECLS

#endif
