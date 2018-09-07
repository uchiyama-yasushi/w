#if 0
/*
Page size: 8KB
Page table entry size: 8B
8MB / page table page.
256 page table page entry required. (total 2048byte)
page offset:
...................||||||||||||| 12 11 10 9 8 7 6 5 4 3 2 1 0 [0x00001fff] 8191
page table page offset. shift=13 mask=0x3ff
.........||||||||||............. 22 21 20 19 18 17 16 15 14 13 [0x007fe000] 8380416
page table page index.  shift=23, mask=0xff
.||||||||....................... 30 29 28 27 26 25 24 23 [0x7f800000] 2139095040
*/
#define	PAGE_SIZE		0x2000
#define	PTE_SIZE		8
#define	PTP_OFFSET		0x7fe000
#define	PTP_OFFSET_SHIFT	13
#define	PTP_OFFSET_MASK		0x3ff
#define	PTP_INDEX		0x7f800000
#define	PTP_INDEX_SHIFT		23
#define	PTP_INDEX_MASK		0xff
#define	PTP_ENTRY		256
#else
#include "pte_page_config.h"
#endif

#define	ROUND_PAGE(x)	(((x) + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1))
#define	TRUNC_PAGE(x)	((x) & ~(PAGE_SIZE - 1))


struct pte
{
  uint32_t ptel;
  uint32_t ptea;
};
