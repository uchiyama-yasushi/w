
/*-
 * Copyright (c) 2010 UCHIYAMA Yasushi.  All rights reserved.
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

// SH2A: SH7262
#ifndef _SH2A_CACHE_H_
#define	_SH2A_CACHE_H_
// D-cache:
//		  write-thru write-back
// write-allocate    no         yes
//
// write-back buffer 16byte.

#define	SH2A_ICACHE_SIZE	8192
#define	SH2A_DCACHE_SIZE	8192

#define	SH4A_CACHE_LINESZ	16
#define	SH4A_CACHE_WAY		4

// SH2A_DCACHE_SIZE / SH2A_CACHE_LINESZ / SH2A_CACHE_WAY = 128
#define	SH4A_CACHE_ENTRY	128	// entry/way

#define	ROUND_CACHELINE_16(x)	(((x) + 15) & ~15)
#define	TRUNC_CACHELINE_16(x)	((x) & ~15)

// Cache control register
#define	SH2A_CCR1	((volatile uint32_t *)0xfffc1000)
#define	SH2A_CCR2	((volatile uint32_t *)0xfffc1004)
// Instruction Cache Invalidate (all entry)
#define	 CCR1_ICI	(1 << 11)
// Instruction Cache Enable
#define	 CCR1_ICE	(1 << 8)
// Operand Cache Invalidate (all entry)
#define	 CCR1_OCI	(1 << 3)
// Write Thru (1:write-thru, 0:write-back)
#define	 CCR1_WT	(1 << 1)
// Operand Cache Enable
#define	 CCR1_OCE	(1 << 0)

// Cache Lock Enable
#define	 CCR2_LE	(1 << 16)
// Cache lock mode.
#define	 CCR2_W3LOAD	(1 << 9)
#define	 CCR2_W3LOCK	(1 << 8)
#define	 CCR2_W2LOAD	(1 << 1)
#define	 CCR2_W2LOCK	(1 << 0)


//
// Memory-Maped Cache Configuratiion
//

// I-cache address array
#define	SH2A_CCIA	0xf0000000	//-0xf07ffff
// address-field
#define	 CCIA_A			0x8	// Associative bit.
#define	 CCIA_ENTRY_MASK	0x7f
#define	 CCIA_ENTRY_SHIFT	4
#define	 CCIA_WAY_MASK		0x3
#define	 CCIA_WAY_SHIFT		11
// data-field (physical tag)
#define	 CCIA_V			0x1	// Valid bit
#define	 CCIA_TAG_MASK		0x3ffff
#define	 CCIA_TAG_SHIFT		11

// I-cache data array
#define	SH2A_CCID	0xf1000000	//-0xf17fffff
// address-field
#define	 CCID_LINE_MASK		0x3	// line size is 16B
#define	 CCID_LINE_SHIFT	2
#define	 CCID_ENTRY_MASK	0x7f	// 128 entry
#define	 CCID_ENTRY_SHIFT	4
#define	 CCID_WAY_MASK		0x3	// 4way
#define	 CCID_WAY_SHIFT		11
// data-field (cached data)
//


// D-cache address array
#define	SH2A_CCDA	0xf0800000	//-0xf0ffffff
// address-field
#define	 CCDA_A			0x8	// Associative bit.
#define	 CCDA_ENTRY_MASK	0x7f
#define	 CCDA_ENTRY_SHIFT	4
#define	 CCDA_WAY_MASK		0x3
#define	 CCDA_WAY_SHIFT		11
// data-field (physical tag)
#define	 CCDA_V			0x1	// Valid bit
#define	 CCDA_U			0x2	// Dirty bit
#define	 CCDA_TAG_MASK		0x3ffff
#define	 CCDA_TAG_SHIFT		11

// D-cache data array
#define	SH2A_CCDD	0xf1800000	//-0xf1ffffff
// address-field
#define	 CCDD_LINE_MASK		0x3	// line size is 16B
#define	 CCDD_LINE_SHIFT	2
#define	 CCDD_ENTRY_MASK	0x7f	// 128 entry
#define	 CCDD_ENTRY_SHIFT	4
#define	 CCDD_WAY_MASK		0x3	// 4way
#define	 CCDD_WAY_SHIFT		11
// data-field (cached data)
//

__BEGIN_DECLS
void sh2a_cache_config (void);

void sh2a_cache_enable (bool);
void sh2a_cache_disable (void);

void sh2a_icache_sync_all (void);
void sh2a_icache_sync_range (vaddr_t, vsize_t);
void sh2a_icache_sync_range_index (vaddr_t, vsize_t);

void sh2a_dcache_wbinv_all (void);
void sh2a_dcache_wbinv_range (vaddr_t, vsize_t);
void sh2a_dcache_inv_range (vaddr_t, vsize_t);
void sh2a_dcache_wb_range (vaddr_t, vsize_t);
void sh2a_dcache_inv_range_index (vaddr_t, vsize_t);
void sh2a_dcache_wbinv_range_index (vaddr_t, vsize_t);
#ifdef DEBUG
void sh2a_dcache_array_dump (vaddr_t, vsize_t);
#endif

__END_DECLS

#endif
