#include <time.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define START_TIME(t) (t = clock())
#define END_TIME(t) (t = (clock() - t))
#define PRINT_TIME(t) (printf("%f\n", t/(float)CLOCKS_PER_SEC))

#define puti(i) printf("%llu\n", (uint64_t)(i))

#define BOOL(n) !!(n)
#define TO_CHAR_NUM(n) ((n) | 0x30)

#define ROTBF(b, i)				\
  do {						\
    register size_t j, k, c;			\
    for (j = 0, k = i - 1; j < (i >> 1); j++, k--){	\
      c    = b[j];				\
      b[j] = b[k];			\
      b[k] = c;				\
    }						\
  } while(0)

#ifdef __ARM__
#define CVT_INT_STR(b, n, k, i)					\
  do {								\
    b[i] = TO_CHAR_NUM((k = (n * 0x5ull) >> 32));		\
    n    = n - (k * 1000000000);				\
    i   += BOOL(i | k);						\
								\
    b[i] = TO_CHAR_NUM((k = (n * 0x2bull) >> 32));		\
    n    = n - (k * 100000000);					\
    i   += BOOL(i | k);						\
								\
    b[i] = TO_CHAR_NUM((k = (n * 0x1aeull) >> 32));		\
    n    = n - (k * 10000000);					\
    i   += BOOL(i | k);						\
								\
    b[i] = TO_CHAR_NUM((k = (n * 0x10c7ull) >> 32));		\
    n    = n - (k * 1000000);					\
    i   += BOOL(i | k);						\
								\
    b[i] = TO_CHAR_NUM((k = (n * 0xa7c6ull) >> 32));		\
    n    = n - (k * 100000);					\
    i   += BOOL(i | k);						\
								\
    b[i] = TO_CHAR_NUM((k = (n * 0x68db9ull) >> 32));		\
    n    = n - (k * 10000);					\
    i   += BOOL(i | k);						\
								\
    b[i] = TO_CHAR_NUM((k = (n * 0x418938ull) >> 32));		\
    n    = n - (k * 1000);					\
    i   += BOOL(i | k);						\
								\
    b[i] = TO_CHAR_NUM((k = (n * 0x28f5c29ull) >> 32));		\
    n    = n - (k * 100);					\
    i   += BOOL(i | k);						\
								\
    b[i] = TO_CHAR_NUM((k = (n * 0x1999999aull) >> 32));	\
    n    = n - (k * 10);					\
    i   += BOOL(i | k);						\
								\
    b[i] = TO_CHAR_NUM(n);					\
    b[i + 1] = 0;						\
  }\
  while (0)
#else
#define CVT_ITOSTR(b, n, k, i)			\
  do {						\
    while (n > 0){				\
      k = (n * 0x1999999Aull) >> 32;		\
      b[i++] = TO_CHAR_NUM(n - (k * 10));	\
      n = k;					\
    }						\
    ROTBF(b, i);				\
    b[i] = 0;					\
  } while(0)
#endif

static inline  size_t intostr(unsigned int n, char *b){
  register unsigned int k, i;

  i = 0;

  CVT_ITOSTR(b, n, k, i);

  return i;
}


static inline void _intostr(unsigned int n, char *b){
  register unsigned int k, i;

  i = 0;
  while (n > 0) {
    k = n / 10;
    b[i++] = TO_CHAR_NUM(n - (k * 10));
    n = k;
  }
  ROTBF(b, i);
  b[i] = 0;
}


int main(void) {
  clock_t t1, t2, t3;
  char a[11], b[11], c[11];
  unsigned int n;
  int i;

  n = UINT_MAX;
  //n = 12453;
  clock();

  START_TIME(t1);
  for (i = 0; i < 1000; i++)
    intostr(n, a);
  END_TIME(t1);

  clock();
  START_TIME(t2);
  for (i = 0; i < 1000; i++)
    intostr(n, b);
  END_TIME(t2);

  clock();
  START_TIME(t3);
  for (i = 0; i < 1000; i++)
    _intostr(n, c);
  END_TIME(t3);

  puts(a);
  puts(b);
  puts(c);
  PRINT_TIME(t1);
  PRINT_TIME(t2);
  PRINT_TIME(t3);
  return 0;
}

