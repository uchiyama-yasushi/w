	.arch i8086
	.section .text
	.code16
	.balign 4
	cld			/* increment.*/
	movw	$_test_data, %si
loop:
	lodsb
	cmp	$0,	%al
	jz	end
	movb	$0x0e,	%ah
	movb	$0x00,	%bh
	movb	$0x07,	%bl
	int	$0x10
	jmp	loop
end:
	jmp	end

	.balign 4
	.section .data
	.global _test_data
_test_data:
	.string "ohayo"
