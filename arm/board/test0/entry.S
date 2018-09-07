	.section	.vector, "ax"
	.align	2
	.global	vector_table
vector_table:
	b	func0		// 0xea00007e
	.long	0x00
	.long	0x00
	.long	0x00
	.long	0x00
	.long	0x00
	.long	0x00
	.long	0x00
