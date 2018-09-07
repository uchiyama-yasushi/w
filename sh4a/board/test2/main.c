#include <sys/types.h>
#include <sys/system.h>
#include <sys/console.h>
#include <console_machdep.h>
#include <sys/shell.h>

//R0P7786LC0011RL PLD internal LED register.
#define	LEDCR	((volatile uint8_t *)0xa4000008)
//R0P7786LC0011RL PLD internal switch register.
#define	SWSR	((volatile uint8_t *)0xa400000a)

uint8_t dip_switch (void);
void led (uint8_t);

void
startup ()
{
  extern char bss_start[], bss_end[];
  char *p;

  for (p = bss_start; p < bss_end; p++)
    *p = 0;

  //  int8_t isci_getc (void);

  boot_console_init (TRUE);
  iprintf ("ohayo\n");

  led (dip_switch ());

  shell_init ();
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
