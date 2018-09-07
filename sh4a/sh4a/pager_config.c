
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

/*
  Map area
  U0 2GB
  P0 2GB
  P3 512MB

  Page size 4KB, Page Table Entry 4Byte
  -> 4KB/4Byte = 1024 entry/page
  1024entry maps 1024 * 4KB = 4MB
  2GB / 4MB = 512 page table page.

  page size 0x1000
  page offset: 0x1000 - 1 = 0xfff
  page table page maps 0x1000 * 0x1000 / 4 = 0x400000
  page table page offset = 0x400000 - 1 - 0x3fffff
  page table page index: ~0x3fffff & ~0x80000000 = 0x7fc00000 :512 entry

  Page size nKB, Page Table Entry mByte
  2 * 1024 * 1024 * 1024  / (n * 1024 * n*1024 / m)
  Page table page slot: 2 * 1024 * m / (n * n) 
  1 Page table page maps: n * 1024 * n * 1024 / m

  Page size              4KB   8KB   8KB
  Page talbe entry       4B    4B    8B
  Page table page slot   512   128   256
 */

#include <sys/types.h>
#include <stdio.h>

#define	__BEGIN_MACRO	do {
#define	__END_MACRO	} while (/*CONSTCOND*/0)

#define	__bitdisp(a, s, e, m, c)					\
__BEGIN_MACRO								\
	u_int32_t __j, __j1;						\
	int __i, __s, __e, __n;						\
	__n = sizeof(typeof(a)) * NBBY - 1;				\
	__j1 = 1 << __n;						\
	__e = e ? e : __n;						\
	__s = s;							\
	for (__j = __j1, __i = __n; __j > 0; __j >>=1, __i--) {		\
		if (__i > __e || __i < __s) {				\
			printf("%c", a & __j ? '+' : '-');		\
		} else {						\
			printf("%c", a & __j ? '|' : '.');		\
		}							\
	}								\
	if (m) {							\
		printf("[%s]", (char*)m);				\
	}								\
	if (c) {							\
		for (__j = __j1, __i = __n; __j > 0; __j >>=1, __i--) {	\
			if (!(__i > __e || __i < __s) && (a & __j)) {	\
				printf(" %d", __i);			\
			}						\
		}							\
	}								\
	printf(" [0x%08x] %d", a, a);					\
	printf("\n");							\
__END_MACRO
#define	bitdisp(a) __bitdisp((a), 0, 0, 0, 1)

int
main (int argc, const char *argv[])
{
  if (argc < 3)
    return 0;

  uint32_t n = atoi (argv[1]);	// Page size (unit KB)
  uint32_t m = atoi (argv[2]);	// PTE size (unit B)

  printf ("/*\n");
  printf ("Page size: %dKB\nPage table entry size: %dB\n", n, m);
  uint32_t page_table_mapsize = n * n / m;
  uint32_t page_table_page_entry = 2 * 1024 * m / (n * n);
  printf ("%dMB / page table page.\n", page_table_mapsize);
  printf ("%d page table page entry required. (total %dbyte)\n",
	  page_table_page_entry, page_table_page_entry * m);

  printf ("page offset:\n");
  uint32_t PAGE_SIZE = n * 1024;
  uint32_t PTE_SIZE = m;
#define	PAGE_OFFSET	(PAGE_SIZE - 1)
// size of 1 page table page can map.
#define	PTP_MAPSIZE	(PAGE_SIZE / PTE_SIZE * PAGE_SIZE )
#define	PTP_OFFSET	((PTP_MAPSIZE - 1) & ~PAGE_OFFSET)
#define	PTP_INDEX	(~(PTP_MAPSIZE - 1) & ~0x80000000)
  bitdisp (PAGE_OFFSET);
  uint32_t ptp_offset_shift = ffs(PTP_OFFSET) - 1;
  printf ("page table page offset. shift=%d mask=0x%x\n", ptp_offset_shift,
	  PTP_OFFSET >> ptp_offset_shift);
  bitdisp (PTP_OFFSET);
  uint32_t ptp_shift = ffs(PTP_INDEX) - 1;
  printf ("page table page index.  shift=%d, mask=0x%x\n", ptp_shift,
	  PTP_INDEX >> ptp_shift);
  bitdisp (PTP_INDEX);
  printf ("*/\n");

  printf ("#define\tPAGE_SIZE\t\t0x%x\n", PAGE_SIZE);
  printf ("#define\tPTE_SIZE\t\t%d\n", PTE_SIZE);
  printf ("#define\tPTP_OFFSET\t\t0x%x\n", PTP_OFFSET);
  printf ("#define\tPTP_OFFSET_SHIFT\t%d\n", ptp_offset_shift);
  printf ("#define\tPTP_OFFSET_MASK\t\t0x%x\n", PTP_OFFSET >> ptp_offset_shift);
  printf ("#define\tPTP_INDEX\t\t0x%x\n", PTP_INDEX);
  printf ("#define\tPTP_INDEX_SHIFT\t\t%d\n", ptp_shift);
  printf ("#define\tPTP_INDEX_MASK\t\t0x%x\n", PTP_INDEX >> ptp_shift);
  printf ("#define\tPTP_ENTRY\t\t%d\n", page_table_page_entry);

  return 0;
}
