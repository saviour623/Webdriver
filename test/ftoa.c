#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <inttypes.h>
#include <stdlib.h>
#include <math.h>

#define PUTI(n) printf( "%" PRId64 "\n", (int64_t)(n))
#define PUTF(f) printf("%f\n", (double)(f))
// N = 2 + |n/LogbB|


#define DBL_ERROR 2.710505431213761e-20

const uint32_t ftoa(double f, char *bf, const int prec) {
  uint32_t k = 0, U = 0;
  double R = f;

  union {double x; uint64_t y;} F = {.x = f};

  int32_t eexp = ((F.y >> 52) & 0b11111111111) - 1023;
  uint64_t mant = (F.y << 12) >> 12;
  int32_t n = -(log2(mant) + eexp + 0.5);

  n = n * .35;
  double M = pow(2, n) / 2;

  PUTI(eexp);
  PUTI(mant);
  PUTF(-1 * log2(mant)-eexp);

  while (1) {
	U = R = (10 * R);
	R = R - U;
	M = M * 10;

	if ( !((R >= M) && (R <= (1. - M))) )
	  break;
	bf[k++] = U | 0x30;
  };
  if (R <= 0.5)
	bf[k] = U | 0x30;
  if (R >= 0.5)
    bf[k] = U + 49;

  bf[++k] = 0;
  return k;
}
int main(void) {
  double f = 0.12312318613712;
  char bf[100];

  ftoa(f, bf, 5);
  puts(bf);
  printf("%.5f\n", f);
  return 0;
}
