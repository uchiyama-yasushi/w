/*
Page size: 4KB
Page table entry size: 8B
2MB / page table page.
1024 page table page entry required. (total 8192byte)
page offset:
....................|||||||||||| 11 10 9 8 7 6 5 4 3 2 1 0 [0x00000fff] 4095
page table page offset. shift=12 mask=0x1ff
...........|||||||||............ 20 19 18 17 16 15 14 13 12 [0x001ff000] 2093056
page table page index.  shift=21, mask=0x3ff
.||||||||||..................... 30 29 28 27 26 25 24 23 22 21 [0x7fe00000] 2145386496
*/
#define	PAGE_SIZE		0x1000
#define	PTE_SIZE		8
#define	PTP_OFFSET		0x1ff000
#define	PTP_OFFSET_SHIFT	12
#define	PTP_OFFSET_MASK		0x1ff
#define	PTP_INDEX		0x7fe00000
#define	PTP_INDEX_SHIFT		21
#define	PTP_INDEX_MASK		0x3ff
#define	PTP_ENTRY		1024
