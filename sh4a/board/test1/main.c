#include <sys/types.h>
#include <sys/system.h>

//R0P7786LC0011RL PLD internal LED register.
#define	LEDCR	((volatile uint8_t *)0xa4000008)
//R0P7786LC0011RL PLD internal switch register.
#define	SWSR	((volatile uint8_t *)0xa400000a)

uint8_t dip_switch (void);
void led (uint8_t);

void
startup ()
{

  while (/*CONSTCOND*/1)
    {
      led (dip_switch ());
    }
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
