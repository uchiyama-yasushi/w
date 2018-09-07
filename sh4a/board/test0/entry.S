	.align	2
	.section .text
	.globl	start
start:
1:	mov.l	.L_led, r0
	mov	#0x4,	r1
	mov.l	r1,	@r0
	bra	1b
	 nop
	/* NOTREACHED */
	.align	2
.L_led:
	.long	0xa4000008

