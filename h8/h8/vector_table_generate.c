// code generator.

#include <sys/types.h>
#include <stdio.h>
typedef int addr_t;
typedef int bool;

#include "vector_table.h"
#include "vector_name.h"
#include "intc.h"

int
main ()
{
  int i, j;

  // Real vector table.
  printf ("__BEGIN_DECLS\n");
  for (j = VECTOR_MIN; j <= VECTOR_MAX; j++)
    {
      printf ("void jmp%02d (void);\n", j);
    }
  printf ("__END_DECLS\n");
  printf ("\nconst interrupt_handler_t vector_table_rom[]\n__attribute ((section (\".intvecs_rom\"))) =\n{\n");

  // Virutual interrupt handler.
  for (j = VECTOR_MIN; j <= VECTOR_MAX; j++)
    {
      printf ("  jmp%02d, /* %2d 0x%02x %s */\n", j, j, vector[j].addr,
	      vector[j].name);
    }
  printf ("};\n");


  printf ("#include <asm/machine.h>\n");
  printf ("	H8ARCH\n");
  printf ("	.section .vector_link_table\n");

  for (j = VECTOR_MIN; j <= VECTOR_MAX; j++)
    {
      printf ("	.globl _jmp%02d\n", j);
      printf ("_jmp%02d: .long	0\n", j);
    }
}
