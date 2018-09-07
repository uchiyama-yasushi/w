
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

#ifndef _FILESYSTEM_MBR_H_
#define	_FILESYSTEM_MBR_H_

#define	PHISICAL_DISK_MAX	4

struct mbr_partition
{
  uint8_t boot_indicator;
#define	MBR_PARTITION_ACTIVE	0x80
  // CHS (Cylinder, Head, Sector)
  uint8_t chs_start_head;
  uint8_t chs_start_sector;
  uint8_t chs_start_cylinder;
  uint8_t partition_id;
  uint8_t chs_end_head;
  uint8_t chs_end_sector;
  uint8_t chs_end_cylinder;
  // LBA (Logical Block Address)
  uint32_t lba_start_sector;
  uint32_t lba_nsector;
} __attribute__((packed));

// MBR is little endian
#if BYTE_ORDER == BIG_ENDIAN
#define	MBRVAL16(x)	bswap16(x)
#define	MBRVAL32(x)	bswap32(x)
#else
#define	MBRVAL16(x)	(x)
#define	MBRVAL32(x)	(x)
#endif

#define	MBR_NETBSD	169

__BEGIN_DECLS
// core
struct mbr_partition *mbr_partition_table (const uint8_t *);
int mbr_partition_foreach (struct mbr_partition *,
			   bool (*)(struct mbr_partition *, int));
// util.
bool mbr_partition_id_print (struct mbr_partition *, int);
bool mbr_partition_info (struct mbr_partition *, int);
const char *partition_id_name (int);
__END_DECLS
#endif //_FILESYSTEM_MBR_H_
