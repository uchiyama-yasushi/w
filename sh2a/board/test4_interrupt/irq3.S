	.align	2
	.section .text

	.global _irq3

_irq3_bank:
	mov.l	.L_c_irq3, r0
	mov	sp, r4
	jsr/n @r0
	resbank
	rte
	 nop

_irq3:	// stack
	sts.l	macl, @-sp
	sts.l	mach, @-sp
	stc.l	gbr, @-sp
	movml.l sp, @-sp	// Save PR, R14-R0
	mov.l	.L_c_irq3, r0
	mov	sp, r4
	jsr/n @r0
	movml.l @sp+, sp	// Restore PR, R14-R0
	ldc.l	@sp+, gbr
	lds.l	@sp+, mach
	lds.l	@sp+, macl
	rte
	 nop

	.align	2
.L_c_irq3:
	.long	_c_irq3

