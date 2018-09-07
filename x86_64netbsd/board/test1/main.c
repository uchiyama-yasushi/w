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

__asm(".text;.align 8;.global sys_write;sys_write:;\
mov $4, %eax;mov %rcx, %r10;syscall;retq");

int sys_write(int, const void *, int);

void
_start ()
{
  char str[] = "hello world\n";

  sys_write (1, str, sizeof str - 1);

  while (/*CONSTCOND*/1)
    ;
  //NOTREACHED
}
