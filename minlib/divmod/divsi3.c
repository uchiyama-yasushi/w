#include <sys/types.h>

__BEGIN_DECLS
int32_t __divsi3 (int32_t numerator, int32_t denominator);
uint32_t divmodsi4(int32_t modwanted, uint32_t num, uint32_t den);
__END_DECLS

#define	divnorm(num, den, sign) 		\
{						\
  if (num < 0) 					\
    {						\
      num = -num;				\
      sign = 1;					\
    }						\
  else 						\
    {						\
      sign = 0;					\
    }						\
						\
  if (den < 0) 					\
    {						\
      den = - den;				\
      sign = 1 - sign;				\
    } 						\
}

#define	exitdiv(sign, res) if (sign) { res = - res;} return res;

int32_t
__divsi3 (int32_t numerator, int32_t denominator)
{
  int32_t sign;
  int32_t dividend;

  divnorm (numerator, denominator, sign);

  dividend = divmodsi4 (0,  numerator, denominator);

  exitdiv (sign, dividend);
}

