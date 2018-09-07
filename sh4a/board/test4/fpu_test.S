#include <asm/asm.h>

FUNC (_fpu_exception_init)
	sts	fpscr,	r0
	mov.l	.L_FPU_EN, r1
	or	r1,	r0
	lds	r0,	fpscr
	rts
	 nop
	.align 2
.L_FPU_EN:
	.long	0xf80

FUNC (_fpu_test)
	fmov.s	@r4,	fr0
	fdiv	fr0,	fr1
	rts
	 nop
