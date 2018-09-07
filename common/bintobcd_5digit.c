#include <sys/types.h>

__BEGIN_DECLS
uint32_t bintobcd_5digit (int32_t bin);
__END_DECLS

uint32_t
bintobcd_5digit (int32_t bin)
{
  int i, j, k, s;
  uint32_t bcd;

  if ((s = sign (bin)) < 0)
    bcd = 0x80000000;
  else
    bcd = 0x00000000;

  for (i = 10000, k = 16; k >= 0; i /= 10, k -= 4)
    {
      j = bin / i;
      bcd |= ((j * s) << k);
      bin -= j * i;
    }

  return bcd;
}
