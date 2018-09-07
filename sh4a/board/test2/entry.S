	.align	2
	.section .text
	.globl	start
start:
1:	mov.l	.L_led, r0
	mov	#0x4,	r1
	mov.l	r1,	@r0
	mov.l	.L_startup, r0
	mov.l	.L_stack_start, r15
	jmp	@r0
	 nop
	/* NOTREACHED */
	.align	2
.L_led:
	.long	0xa4000008
.L_startup:
	.long	_startup
.L_stack_start:
	.long	_stack_start

