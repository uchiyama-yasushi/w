#include <sys/types.h>

long
random ()
{
  static int32_t r = 1;

  r = r * 1103515245 + 12345;
  return r & 0x7fffffff;
}
