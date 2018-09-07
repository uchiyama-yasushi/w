#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
uint32_t
prefer(uint32_t hint, uint32_t addr)
{
	uint32_t va = addr+((hint - addr) & 0x1fff);


	if ((va & (0xff << 5)) != (hint & (0xff << 5))) {
		printf ("(1)%x %x->%x\n", hint, addr, va);
		printf ("INDEX ERROR!!\n");
	}
	return va;
}

uint32_t
prefer2(uint32_t hint, uint32_t addr)
{
	uint32_t ca1 = hint & (0xff << 5);
	uint32_t ca2 = addr & (0xff << 5);
	uint32_t va = addr + (ca1 - ca2);

	if ((va & (0xff << 5)) != (hint & (0xff << 5))) {
		printf ("(2)%x %x->%x\n", hint, addr, va);
		printf ("INDEX ERROR!!\n");
	}
	return va;
}


int
main (int argc, const char *argv[])
{
#if 0
	if (argc < 3)
		return 0;

	prefer(atoi(argv[1]), atoi(argv[2]));
#else
	while (1) {
		uint32_t r0 = (uint32_t)rand ();
		uint32_t r1 = (uint32_t)rand ();

		if (prefer(r0, r1) == prefer2(r0, r1))
			printf ("%x %x\n", r0, r1);
	}
#endif
	return 0;
}
