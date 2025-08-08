#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>

#define START_TIME(t) ((t) = clock())
#define END_TIME(t) ((t) = clock() - (t))
#define PRINT_TIME(t) (printf("%f\n", (t)/(float)CLOCKS_PER_SEC))
#define puti(i) printf("%ld\n", (unsigned long)(i))

const long reciprocalLkupTable[10] = {
				      0x1999999A,/* 0.1 */
				      0x28f5c29, /* 0.001*/
				      0x418938, /* 0.0001 */
				      0x68db9, /* 0.00001 */
				      0xa7c6, /* 0.000001 */
				      0x10c7,
				      0x1ae,
				      0x2b,
				      0x5 /* 0.0000000001 */
};

const long pow10kupTable[10] = {
			       10,
			       100,
			       1000,
			       10000,
			       100000,
			       1000000,
			       10000000,
			       100000000
};

unsigned long tStr(unsigned long n) {
  volatile unsigned long i, dv, rm;

  i = 9;

  while (i-- > 0) {
    dv = (n * reciprocalLkupTable[i]) >> 32;
    n = (n - (dv * pow10kupTable[i]));
  }
  return 0;
}

unsigned long tStr2(long n) {
 volatile unsigned long i, dv, rm;

 while (n > 0) {
   dv = n / 10;
   rm = n - (dv * 10);
   n = dv;
 }
  return 0;
}

unsigned int tStr3(long n) {
 volatile unsigned long i, dv, rm;

 while (n > 0) {
   dv = (n * 0x1999999a) >> 32 ;
   rm = n - ((dv << 3) + (dv << 1));
   n = dv;
 }
  return 0;
}

int main(void) {
  clock_t t;
  int i;
  START_TIME(t);
  for (i = 10; i > 0; i--)
    tStr(348785); 
  END_TIME(t);

  clock_t t2;
  START_TIME(t2);
  for (i = 10; i > 0; i--)
    tStr2(348785);
  END_TIME(t2);

  clock_t t3;
  START_TIME(t3);
  for (i = 10000; i > 0; i--)
    tStr3(348785);
  END_TIME(t3);

  PRINT_TIME(t);
  PRINT_TIME(t2);
  PRINT_TIME(t3);
  //puti(345);
}

/*
0x44b82fa09b5a5,

			     0x2af31dc4611874,

			     0x1ad7f29abcaf485,

			     0x10c6f7a0b5ed8d37,

			     0xa7c5ac471b478423,

			     0x68db8bac710cb295f,


			     0x4189374bc6a7ef9db2,
			  0x28f5c28f5c28f5c28fc,
			  0x1999999999999999999a
*/
