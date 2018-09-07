
int
a (int a0, int a1, int a2, int a3, int a4)
{
  return a0+a1+a2+a3+a4;
}

void
b ()
{
  float f0 = 1.0;
  float f1 = (float)a (0, 1, 2, 3, 4);
  f0 /= f1;
}

/*
	.file	"a.c"
	.text
	.text
	.align 1
	.global	_a
	.type	_a, @function
_a:
	mov.l	r14,@-r15
	mov	r15,r14
	mov	r5,r0
	add	r4,r0
	add	r6,r0
	add	r7,r0
	mov.l	@(4,r14),r1
	add	r1,r0
	mov	r14,r15
	rts	
	mov.l	@r15+,r14
	.size	_a, .-_a
	.align 1
	.global	_b
	.type	_b, @function
_b:
	mov.l	r14,@-r15
	mov	r15,r14
	mov	r14,r15
	rts	
	mov.l	@r15+,r14
	.size	_b, .-_b
	.ident	"GCC: (GNU) 4.3.2"
*/
