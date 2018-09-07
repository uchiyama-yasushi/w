#include <sys/types.h>

__BEGIN_DECLS
int32_t __umodsi3 (uint32_t numerator, uint32_t denominator);
uint32_t divmodsi4(int32_t modwanted, uint32_t num, uint32_t den);
__END_DECLS

int32_t
__umodsi3 (uint32_t numerator, uint32_t denominator)
{

  int32_t modul;

  modul= divmodsi4 (1,  numerator, denominator);
  return modul;
}
