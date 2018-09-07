#include <sys/shell.h>
#include <sys/board.h>

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

__asm(".text;.align 8;.global sys_read;sys_read:;\
mov $3, %eax;mov %rcx, %r10;syscall;retq");

int sys_write(int, const void *, int);
int sys_read(int, void *, int);

void
o_start ()
{
  char str[] = "ohayo desu!\n";
  static char str1[] = "hello world\n";

  sys_write (1, str, sizeof str - 1);
  sys_write (1, str1, sizeof str1 - 1);
  char buf[64];

  while (/*CONSTCOND*/1)
    {
      int i= sys_read (0, buf, 1);
      if (i)
	sys_write (1, buf, 1);
      sys_write (1, str, sizeof str - 1);
    }
    ;
  //NOTREACHED
}

#include <sys/types.h>
#include <stdio.h>
#include <sys/console.h>

BOOT_CONSOLE_DECL (userland);

//void *bss_end;
void
board_reset (uint32_t cause)
{
  cause=cause;
  while (/*CONSTCOND*/1)
    ;
  //NOTREACHED
}

int
_start ()
{
  _boot_console_init (&userland_console_ops);

  printf ("hello world\n");
  shell_prompt (stdin, stdout);

  return 0;
}

int8_t
userland_getc (void *arg)
{
  arg=arg;
  int8_t c;
  while (sys_read (0, &c, 1) <= 0)
    ;
  if (c == '\n')
    c = '\r';

  return c;
}

int8_t
userland_getc_cont (void *arg, continuation_func_t cont __attribute__((unused)))
{

  return userland_getc (arg);
}

void
userland_putc (void *arg __attribute__((unused)), int8_t c)
{

  md_uart_putc1 (c);
}

int8_t
userland_ungetc (void *arg, int8_t c)
{
  struct userland_boot_console *cons = (struct userland_boot_console *)arg;

  if (c == EOF)
    return EOF;

  cons->save = c;

  return c;
}

void
md_uart_putc1 (int8_t c)
{

  sys_write (1, &c, 1);
}
