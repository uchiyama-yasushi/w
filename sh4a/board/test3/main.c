#include <sys/types.h>
#include <sys/system.h>
#include <sys/console.h>
#include <console_machdep.h>
#include <sys/shell.h>
#include <cpu.h>

//R0P7786LC0011RL PLD internal LED register.
#define	LEDCR	((volatile uint8_t *)0xa4000008)
//R0P7786LC0011RL PLD internal switch register.
#define	SWSR	((volatile uint8_t *)0xa400000a)

int current_thread; //XXX
int thread_context_switch; //XXX

uint8_t dip_switch (void);
void led (uint8_t);

SHELL_COMMAND_DECL (test);
SHELL_COMMAND_DECL (exception);

void
startup ()
{
  extern char bss_start[], bss_end[];
  uint32_t r;
  char *p;

  for (p = bss_start; p < bss_end; p++)
    *p = 0;

  boot_console_init (TRUE);
  iprintf ("ohayo\n");

  led (dip_switch ());

  shell_init ();

  shell_command_register (&test_cmd);
  shell_command_register (&exception_cmd);

  CPU_GET_SR (r);
  cpu_dump_sr (r);
#if 0 // To disable, compile with -m2
  r |= SR_FD;	// FPU disable.
  CPU_SET_SR (r);
#endif
  exception_enable ();

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

uint8_t
dip_switch ()
{

  return *SWSR;
}

void
led (uint8_t sw)
{
  int i;

  for (i = 1; i < 16; i <<= 1)
    {
      uint8_t r = i | i << 4;
      if (sw & i)
	*LEDCR |= r;
      else
	*LEDCR &= ~r;
    }
}

void
board_reset () {}

uint32_t
exception (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
  // __asm volatile (".short 0xfffd");	// 0x180
  // __asm volatile ("trapa #33");	// 0x160
  //  __asm volatile ("1: nop; bra 1b; mov.l 2f, r0;.align 2;2: .long 0xaa");//0x1a0
  //  __asm volatile (".byte 0xff"); // 0x100
  //  __asm volatile ("mov r15, r0;add #1, r0;mov.l @r0, r1");// 0xe0
  //  __asm volatile ("mov r15, r0;add #1, r0;mov.l r1, @r0"); //0x100
  //  __asm volatile ("fneg fr0");	// 0x800
  //  __asm volatile ("bra 1f;fneg fr0;1:");// 0x820
  extern void fpu_exception_init (void);
  uint32_t r;
  __asm volatile ("sts	fpscr, %0" : "=r" (r));
  printf ("%x\n", r);

  fpu_exception_init ();

  __asm volatile ("sts	fpscr, %0" : "=r" (r));
  printf ("%x\n", r);

  /*
gcc-4.6.0
main.c:(.text+0x78): undefined reference to `__fpscr_values'
main.c:(.text+0x84): undefined reference to `__fpscr_values'
   */
  //  float i = 0.;
  //  __asm volatile ("fmov.s %0, fr0;fdiv fr0, fr1":: "m"(i)); // 0x120
  //  printf ("skip instruction\n");

  return 0;
}

uint32_t
test (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
  uint32_t r;

  CPU_GET_SR (r);
  cpu_dump_sr (r);

  return 0;
}
