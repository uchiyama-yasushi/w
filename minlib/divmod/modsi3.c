#include <sys/types.h>

__BEGIN_DECLS
uint32_t divmodsi4(int32_t modwanted, uint32_t num, uint32_t den);
int32_t __modsi3 (int32_t numerator, int32_t denominator);
__END_DECLS

int32_t
__modsi3 (int32_t numerator, int32_t denominator)
{
  int32_t sign = 0;
  int32_t modul;

  if (numerator < 0)
    {
      numerator = -numerator;
      sign = 1;
    }
  if (denominator < 0)
    {
      denominator = -denominator;
    }

  modul =  divmodsi4 (1, numerator, denominator);
  if (sign)
    return - modul;
  return modul;
}
