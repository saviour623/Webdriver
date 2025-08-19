#include <stdio.h>
#include <math.h>
#include <time.h>

#define START(t) ((t) = clock())
#define STOP(t) ((t) = clock() - t)
#define OUT(s, x, t) printf("%s: %.20f\nTime: %f\n", s, x, (t)/(double)CLOCKS_PER_SEC)
#define LOOP(n, f, k) do { for (long long int i = n; i > 0; i--) ( k = f);} while (0) 

 
static inline __attribute__((always_inline)) double exp__(double x) {
  const double x2 = x * x;

  // Approximate Ln(dp) using taylor series {dp: [0, 1)}
  return 1. + (x + (0.5 * x2) * (1. + ((0.333333333333333 * x) + ((0.17 * x2) * ((0.25 * x2) + (0.17 * x2) * x)))));
}

 
static inline __attribute__((always_inline)) double exp1__(double x) {
  const double x2 = x * x;

  return 1.0 + (2*x) / (2 - x + (x2) / (6 + (x2) / (10 + (x2) / 14)));
}

static inline __attribute__((always_inline)) double exp2__(double x) {
  double x2 = x * x;

  return 1. + x * 2. / ((2. - x) + (x2 * (140. + x2) / (840. + x2 * 20.)));
}

static inline __attribute__((always_inline)) double exp3__(const double x) {
   const double x2 = x * x, a = 840. + 20. * x2;

   return 1. + 2. * x * a / ((2. - x) * a + x2 * (140. + x2));
}

#include <stdio.h>
#include <math.h>

#define N 16
#define LN2_DIV_N (0.04332169878499658)  // ln(2)/16
#define INV_LN2_N (23.08336743743)      // 16/ln(2)

// Precomputed table for 2^(k/N), k = 0..(N-1)
static const double pow2_table[N] = {
    1.0000000000000000, 1.0442737824274138, 1.0905077326652577, 1.1387886347566916,
    1.189207115002721, 1.241857812073484, 1.2968395546510096, 1.3542555469368927,
    1.4142135623730951, 1.4768261459394993, 1.5422108254079407, 1.6104903319492543,
    1.681792830507429, 1.7562521603732995, 1.8340080864093425, 1.9152065613971474
};

// Approximate exp(r) using 5-term Taylor expansion
double exp_poly(double r) {
    return 1 + r * (1 + r * (0.5 + r * (1.0/6 + r * (1.0/24 + r * (1.0/120)))));
}


double fast_exp(double x) {
    int k = (int)(x * INV_LN2_N);
    int idx = k & (N - 1);  // mod N
    double hi = k * LN2_DIV_N;
    double r = x - hi;

    double base = pow2_table[idx];
    double exp_r = exp2__(r);

    // Adjust with 2^(k/N) = 2^(k/N) = 2^(k >> log2(N)) * pow2_table[k % N]
    int shift = 0;
    return ldexp(base * exp_r, shift);  // base * exp(r) * 2^shift
}

#define putf(n) printf("%.25Lf\n", (long double)(n))
static inline __attribute__((always_inline)) double tst_(double x) {
  const double x2 = 1e15;
  double k = 0.9744574336753467;
  double c = k * 0.1;

  x = k * x2;
  x = (x - floor(x)) * 1e-15;
  x = 0.00000000000000535;
  
  putf(k);
  putf(x);
  putf(exp(k));
  putf(pow(exp(c), 10) * exp(x));
   return 0.0;
}

static inline void base16(unsigned int x) {
  
}
int main(void) {
  volatile double f, x = 0.99;
  volatile clock_t _1, _2, _3, _4, _5;
  int n = 10000;

  putf(fast_exp(x) * 2);

  START(_1);
  LOOP(n, exp(x), f);
  STOP(_1);
  OUT("exp", f, _1);

  START(_2);
  LOOP(n, exp__(x), f);
  STOP(_2);
  OUT("exp__", f, _2);

  START(_3);
  LOOP(n, exp1__(x), f);
  STOP(_3);
  OUT("exp_CF_4div", f, _3);

  START(_4);
  LOOP(n, exp2__(x), f);
  STOP(_4);
  OUT("exp_CF_2div", f, _4);

  START(_5);
  LOOP(n, exp3__(x), f);
  STOP(_5);
  OUT("exp_CF_1div", f, _5);

  return 0;
}
