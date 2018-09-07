
/*-
 * Copyright (c) 2011 UCHIYAMA Yasushi.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define	ELF_NOTE_NETBSD_NAMESZ		7
#define	ELF_NOTE_NETBSD_DESCSZ		4
#define	ELF_NOTE_TYPE_NETBSD_TAG	1
#define	ELF_NOTE_NETBSD_NAME		"NetBSD\0\0"
#define	__NetBSD_Version__	599002400	/* NetBSD 5.99.24 */

#define	__STRING(x)	#x
#define	__S(x)	__STRING(x)
__asm(
	".section\t\".note.netbsd.ident\", \"a\"\n"
	"\t.p2align\t2\n\n"

	"\t.long\t" __S(ELF_NOTE_NETBSD_NAMESZ) "\n"
	"\t.long\t" __S(ELF_NOTE_NETBSD_DESCSZ) "\n"
	"\t.long\t" __S(ELF_NOTE_TYPE_NETBSD_TAG) "\n"
	"\t.ascii\t" __S(ELF_NOTE_NETBSD_NAME) "\n"
	"\t.long\t" __S(__NetBSD_Version__) "\n\n"

	"\t.previous\n"
	"\t.p2align\t2\n"
);

#define	SYSCALL_NETBSD(n, x)	\
__asm (".text;			\
	.align 8;		\
	.global " #x ";		\
	" #x ":;		\
	mov $ " #n ", %eax;	\
	mov %rcx, %r10;		\
	syscall;		\
	retq");

SYSCALL_NETBSD (4, sys_write);
SYSCALL_NETBSD (3, sys_read);
