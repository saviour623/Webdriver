#include <stdio.h>
#include <float.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>

#define puti(i)  printf("%llu\n", (long long)(i))
#define putd(d)  printf("%lld\n", (long long)(d))
#define puthx(x) printf("%#llx\n", (long long)(x))
#define putf(f)  printf("%.30f\n", (double)(f))
#define PASS(...) (void *)0
#define expOf(...) true

typedef union {
    uint64_t N;
    double   F;
  } split_t;

void split(double a, double *ahi, double *alo) {
  //10101111 1111111 11111101
  split_t S = {.F = a};
  const int16_t e = (S.N >> 52u) - 1023;

  S.N = ((1023ull + (e>>1)) << 52) | (S.N & 0xffffffc000000);
  *ahi = S.F;
  S.F = a;
  S.N = ((1023ull + (e & 1)) << 52) | (S.N & 0x3ffffff) << 25;
  *alo = S.F;
}

void construct(double *a, double ahi, double alo) {
  //10101111 1111111 11111101
  split_t S;
  uint64_t s1, s2;
  uint64_t e1, e2;

  S.F = ahi;
  e1 = ((S.N >> 52u) - 1023) << 52;
  s1 = e1 | ((S.N & 0xfffffffffffff) & ~0x3ffffff);
  S.F = alo;
  e2 = ((S.N >> 52u) - 1023) << 52;
  s2 = e2 | ((S.N & 0xfffffffffffff) >> 26);

  S.N = ((e2 + (((e2 & 1) + e1) >> 1) + 1023) << 52) | (s2 + s1);
  *a = S.F;
}

void add(double a, double b, double *s) {
  //10101111 1111111 11111101
  double ahi, alo, bhi, blo;

  split(a, &ahi, &alo);
  split(b, &bhi, &blo);

  ahi = ahi + bhi;
  alo = alo + ahi;

  construct(s, ahi, alo);
}


int main(void) {
  double a, b;

  a = 17424121.2242343;
  b = 124122.3623434;

  putf(a+b);
  add(a, b, &a);
  putf(a);
}
