#include <sys/types.h>

__BEGIN_DECLS
int32_t __mulsi3 (uint32_t a, uint32_t b);
__END_DECLS

int32_t
__mulsi3 (uint32_t a, uint32_t b)
{
  int32_t r = 0;

  while (a)
    {
      if (a & 1)
        r += b;
      a >>= 1;
      b <<= 1;
    }

  return r;
}
