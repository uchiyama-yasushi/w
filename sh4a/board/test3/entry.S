	.align	2
	.section .text
	.globl	start
start:
1:	mov.l	.L_led, r0
	mov	#0x4,	r1
	mov.l	r1,	@r0
// Set VBR
	mov.l	.L_VBR, r0
	ldc	r0,	vbr
// Change BANK0
	stc	sr,	r1
	mov.l	.L_SR_RB0, r0
	and	r0,	r1
	ldc	r1,	sr
// Jump to C routine.
	mov.l	.L_startup, r0
// Set stack
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
.L_VBR:
	.long	_VBR
.L_SR_RB0:
	.long	0xdfffffff
