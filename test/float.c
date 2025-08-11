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
#define putf(f)  printf("%.23f\n", (double)(f))

typedef union {
  float *F;
  uint32_t *N;
} conv_t;

const float asfloat(uint32_t n) {
  return *(float *)&n;
}

const uint32_t asint(float f) {
  //conv_t cv = { &f };

  return *(uint32_t *)&f;
}

float Alog2(const float f, const float scaled, const uint32_t oneAsFloat) {
  return asfloat(asint((asint(f)-oneAsFloat) + 0) - 0xb800000) + 0.0;/// (1 << 23);
}
#define PASS(...) (void *)0
#define expOf(...) true

typedef union {
  double   F;
  uint64_t N;
} bits_t;

void   reprfloat   (bits_t bits);
double reprexp     (bits_t bits);
double approxLog2  (bits_t bits);
double approxLog10 (bits_t bits);

#define Precalc_Log10b2 3.321928094887362
#define Precalc_Log2b10 0.3010299956639812
#define Precalc_Loge2   0.6931471805599453
#define Precalc_Loge10  2.302585092994046

#define Precalc_1_f_asInt    0x3ff0000000000000ull
#define Precalc_FloatExpBias 0x10000000000000ull
#define Precalc_53shift52    0x350000000000000ull

#define approxLog2p(x) (0.5 * approxLog2(x) + approxLog2p(x*0.66667) * 0.6667)

extern inline double approxLog2(bits_t bits) {
  bits_t FI;

  FI.F = bits.F * 1.3332;

  /*
   * Approximate log10(f) {0 > f <= 2^p-1}
   * We exploit the logarithmic nature of float’s bit pattern (IEEE) such AB * CD (AB=exponent bits; CD=mantissa bits) scaled down to f / 2^p
   * (where p is the precision of the mantissa of f) is an approximate of log2(f)
   * Subtract bias (double=1023) from exponent (without shifts)
   */

  // We use an interative approach calculating half of F
  bits.F = bits.N - Precalc_1_f_asInt; //0x4000000000000000ull;
  // Subtract 52 from the exponent which effectively divides by 2^52 directly
  bits.N = bits.N - Precalc_53shift52;

  FI.F = FI.N - Precalc_1_f_asInt;
  FI.N = FI.N - Precalc_53shift52;

  return bits.F + (FI.F - 0.156);
}

extern inline double approxLog10(bits_t bits) {
  // logaX ~ logbX/logbA or logbX * 1/logbA or 1/logxA

  return (approxLog2(bits) * Precalc_Log2b10); // return log2(f) / log10(2)
}

extern inline void reprfloat(bits_t bits) {
  PASS();
}

#define Precalc_SQRT2              1.4142135623730951
#define Precalc_LogSQRT2b10        0.1505149978319906
#define Precalc_DydFLogb10_f       0.3070925731856877
#define Precalc_D2ydF2Logb10f_Div2 0.1085736204758129
#define Precalc_AllOnes52bits      0xfffffffffffffULL

static inline __attribute__((pure, always_inline)) double pow10__(double x) {
  x = x * x;

  // Calculate 10 ^ 10
  return x * x * x * x * x;
}
static inline __attribute__((pure, always_inline)) double exp__(double x) {
  const double x2 = x * x;

  // Approximate Ln(dp) using taylor series {dp: [0, 1)}
  return 1. + (x + (0.5 * x2) * (1. + ((0.333333333333333 * x) + ((0.17 * x2) * ((0.25 * x2) + (0.17 * x2) * x)))));
}

extern inline double reprexp(bits_t bits) {
  int16_t p2, p10;
  double  F,  dF,  dp, dp2;

  F = bits.F; //keep value

  //  Exponent
  p2 = (bits.N >> 52) - 1023;

  // Mantissa normalized within [1, 2)
  bits.N = (bits.N & Precalc_AllOnes52bits) | (1023ULL << 52);

  // The exponent of F is Approximated using log10(f) where f ~ log(2^p * M)
  // ~ log(2^p * M) ~ log(2^p * M) ~ plog(2) + log(M)
  // where log(M) == ln(M)/ln(10):
  // The input ln(M) to the exponent is approximated using taylor correction about point b and b == SQRT(2)]

  // y = log(f)
  // The Taylor approx. is given as log(b) + plog(2) + df.dy/df - d2f.d2y/df2
  dF = bits.F - Precalc_SQRT2;

  p10 = Precalc_LogSQRT2b10 + ((p2 * Precalc_Log2b10) + ((dF * Precalc_DydFLogb10_f) - (dF * (dF * Precalc_D2ydF2Logb10f_Div2))));

  // Reduce F to [0, 1], given that 0 < F => F’ [F’: Float numbers > F_RNGE]
  // That is, F has to be multiplied by 10^-p such that F < 0 == f and f x 10^p == F
  p2  = Precalc_Log10b2 * p10; // log2(p10)
  // Split 10^p into 2^-p2 * 10dp [dp = p10 - p2]
  dp     = (p10 * Precalc_Loge10) - (p2 * Precalc_Loge2);

  bits.N = (1023ULL + p2) << 52; // 2^-p2

#ifndef MIN_PREC_CALC_FLT
  // reduce dp closer to 0 for better approximation
  dp = .1 * dp;
#endif

  dp = exp__(dp); // e ^ dp ~ 10 ^ (p10 - p2)

#ifndef MIN_PREC_CALC_FLT
  // correct reduction
  dp = pow10__(dp);
#endif

  return F / (bits.F * dp); // F * 10^p or  F * (2^-p2 * 10^dp)
}

static inline void floatRepr(double f) {
  bits_t bitsv;

  bitsv.F = f;
  // absolute(f)
  if (f < 0.0)
    f = -f;
  // Check for -0 or +0
  PASS();
  // check for inf
  PASS();
  // check for nan
  PASS();

  return (expOf(bitsv.N) ? reprfloat(bitsv) : reprexp(bitsv));
}


static inline double exp___(double x) {
  register double x2;

  x2 = x * x;

  return 1 + 2 + x / ((2 - x) + (x2 / (6 + (x2 / (10 + (x2 / 14))))));
}

int main(void) {
  const  uint32_t  oneAsFloat = asint(1.0f);
  const  double    scaleu    = 0x800000;
  const  double    scaled    = 1.0/scaleu;

  const float v = FLT_MAX - 2;
  uint64_t n;
  uint64_t nn;
  double f = 1.0;
  double ff = 2.0;
  double e = 2.718281828459045;
  n = *(uint64_t *)&f;
  nn = *(uint64_t *)&ff;


  bits_t k;
  k.F = 1239342349.0;

  reprexp(k);
}
