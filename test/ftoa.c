#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <inttypes.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <fenv.h>

#define PUTI(n) printf( "%" PRId64 "\n", (int64_t)(n))
#define PUTF(f) printf("%f\n", (double)(f))
// N = 2 + |n/LogbB|

#define abs_(n) ((n) < 0 ? -(n) : (n))
#define INT_ uint64_t
#define itoa_(n,  bf) ((bf[0] = '0'), 1)
#define LARGE_FLOAT(f) 0
#define SUBNORM_FLOAT(f) 0

const uint32_t ftoa(double f, char *bf, int p) {
  int32_t k = 0, j = 0, U = 0;
  double R, M;

  union {
	double x;
	uint64_t y;
  } F = {.x = f};

  int32_t  e = ((F.y >> 52) & 0b11111111111u) - 1023;
  uint64_t m = (F.y << 12) >> 12;
  int32_t  n = -(log2(m) + abs_(e) + 0.5);

  R = f;
  F.y = (uint64_t)(1 + n) << 52;
  
  M = pow(2, n) / 2;

  //PUTF(F.y);
  //PUTF(M);

  if (LARGE_FLOAT(R) || (SUBNORM_FLOAT(R))) {
	//
	R = 0;
  }
  j = itoa_((INT_)(R), bf);
  bf[j] = '.';
  k = j + 1;

  if (R == .0) {
	bf[k++] = '0';
	bf[k] = 0;

	return k;
  }
  if (p < 1)
	goto formating;
  p += 2;

  do {
	U = R = (10 * R);
	R = R - U;
	M = M * 10;

	bf[k++] = U | 0x30;
  } while  ( ((R >= M) && (R <= (1. - M))) && (k < p));

  if (R >= 0.5) {
	char c;

	puts("here");

	c = bf[--k] += 1;
	if (c > '9') {
	  while (k-- && ((c = bf[k]) == '9') || (c == '.')){}
	  bf[k] += 1;
	}
  }

 formating:
  if (k < j) {
	bf[++k] = '.';
	bf[++k] = '0';
	bf[++k] = 0;
  }
  else {
	bf[++k] = 0;
  }
  // formatting
  return k;
}
int main(void) {
  double f;
  char bf[1024];

  for (int i = 0; i < 20; i++) {
	f = 1 / rand();
	ftoa(f, bf, 6);
	puts(bf);
  }
  /*
   ftoa(f, bf, 2);
   puts(bf);
  printf("%f\n", f);
  */
  return 0;
}
/*
E_TONEAREST,  FE_UPWARD,  FE_DOWNWARD,  and  FE_TO‐
       WARDZERO
*/
