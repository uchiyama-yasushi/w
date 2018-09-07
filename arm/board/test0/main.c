

// 64KB Local on-chip SRAM.
#define	RAM	(*(volatile unsigned int *)0x40001000)

void
func0 ()
{

  *(volatile unsigned int *)0xe01fc1a0 |= 1;	// SYS_SCS
  *(volatile unsigned int *)0x3fffc020 = 0x40000;
  *(volatile unsigned int *)0x3fffc030 = 0;
  while (1)
    {
      *(volatile unsigned int *)0x3fffc034 = 0;
      for (RAM = 0; RAM < 10000; RAM++)
	;
      *(volatile unsigned int *)0x3fffc034 = 0x40000;
      for (RAM = 0; RAM < 5000; RAM++)
	;
    }
}

void
func2 ()
{
  while (1)
    ;
}

void
func1 ()
{
  *(volatile unsigned int *)0xe01fc1a0 |= 1;	// SYS_SCS
  *(volatile unsigned int *)0x3fffc020 = 0x40000;
  *(volatile unsigned int *)0x3fffc030 = 0;
  *(volatile unsigned int *)0x3fffc034 = 0;
  while (1)
    ;
}
