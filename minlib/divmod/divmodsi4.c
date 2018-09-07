#include <sys/types.h>

__BEGIN_DECLS
uint32_t divmodsi4(int32_t modwanted, uint32_t num, uint32_t den);
__END_DECLS

uint32_t
divmodsi4(int32_t modwanted, uint32_t num, uint32_t den)
{
  int32_t bit = 1;
  int32_t res = 0;

  while (den < num && bit && !(den & (1L<<31)))
    {
      den <<=1;
      bit <<=1;
    }
  while (bit)
    {
      if (num >= den)
	{
	  num -= den;
	  res |= bit;
	}
      bit >>=1;
      den >>=1;
    }
  if (modwanted) return num;
  return res;
}
