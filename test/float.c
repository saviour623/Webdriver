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
#define putf(f)  printf("%.20f\n", (double)(f))

#if defined(__GNUC__) || defined(__clang__)
    #define FORCEI_PURE __attribute__((pure, always_inline))
#elif defined(_MVSC) || defined(_WIN32)
    #define FORCEI_PURE __forceinline
#else
    #define FORCEI_PURE
#endif
#define INLINE(T) __inline__ FORCEI_PURE T

#define PASS(...) (void *)0
#define expOf(...) true

typedef union {
  double   F;
  uint64_t N;
} bits_t;

typedef struct {
  bits_t  fmt_Num;
  int16_t fmt_Exp;
  int16_t fmt_Err;
} fmt;

void   reprfloat   (bits_t bits);
double reprexp     (bits_t bits);
double approxLog2  (bits_t bits);
double approxLog10 (bits_t bits);
double xpow10__     (double x);
double exp__       (const double x);

#define Precalc_Log10b2 3.321928094887362
#define Precalc_Log2b10 0.3010299956639812
#define Precalc_Loge2   0.6931471805599453
#define Precalc_Loge10  2.302585092994046

#define Precalc_1_f_asInt    0x3ff0000000000000ull
#define Precalc_FloatExpBias 0x10000000000000ull
#define Precalc_53shift52    0x350000000000000ull

#define Precalc_SQRT2              1.4142135623730951
#define Precalc_LogSQRT2b10        0.1505149978319906
#define Precalc_DydFLogb10_f       0.3070925731856877
#define Precalc_D2ydF2Logb10f_Div2 0.1085736204758129
#define Precalc_AllOnes52bits      0xfffffffffffffULL

#define approxLog2p(x) (0.5 * approxLog2(x) + approxLog2p(x*0.66667) * 0.6667)

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

extern INLINE(double) approxLog2(bits_t bits) {
  bits_t FI;

  /*
   * Approximate log10(f) {0 > f <= 2^p-1}
   * We exploit the logarithmic nature of float’s bit pattern (IEEE) such AB * CD (AB=exponent bits; CD=mantissa bits) scaled down to f / 2^p
   * (where p is the precision of the mantissa of f) is an approximate of log2(f)
   */
  // Second iteration requires F = F * c {c: 0.6666} (FLoating Point Tricks By Blinn), however F * x may reduce to F < 0. Since this is below our limit, it is trouble. However, we can perform F * 2c, relying on the fact that LogN(F) = LogN(F * N) - 1 {F: F => 1} or LogN(F / N) - 1 {F: F < 1}
  // So Log2(F * c * 2) ~ 1 - log2(F * C) (Keeping F above 0)
  FI.F = bits.F * 1.3332;
  //Subtract bias (double=1023) from exponent (without shifts)

  bits.F = bits.N - Precalc_1_f_asInt; //0x4000000000000000ull;
  // Subtract 52 from the exponent which effectively divides by 2^52 directly
  bits.N = bits.N - Precalc_53shift52;

  FI.F = FI.N - Precalc_1_f_asInt;
  FI.N = FI.N - Precalc_53shift52;

  return bits.F + (FI.F - 0.156);
}

extern INLINE(double) approxLog10(bits_t bits) {
  // logaX ~ logbX/logbA or logbX * 1/logbA or 1/logxA

  return (approxLog2(bits) * Precalc_Log2b10); // return log2(f) / log10(2)
}

extern inline void reprfloat(bits_t bits) {
  PASS();
}


extern INLINE(double) xpow10__(double x) {
  x = x * x;

  // Calculate x ^ 10
  return x * x * x * x * x;
}

extern INLINE(double) exp__(const double x) {
  // Approximate exp(x) for [0, 1) using a condensed approach to the proposed continued fraction method by ...
  // exp = 1 + 2x / (2 - x + x^2 / (6 + x^2 / (10 + x^2 / 14 + ...
  // =~ 1 + 2x / [[(2 - x)(840 + 20x^2) + (140 + x^2)x^2] / 840 + 20x^2]  (5 terms only)
  // Guarantees >8 decimal precision for x in [0, 1) and almost the exact value returned by the stdmath exp for x in [0, 0.1)
  const double x2 = x * x, a = 840. + 20. * x2;
  //    exp(x) ~ 1 + 2x / [((2 - x)*A + (140 + x^2)*x^2) / A]      (1)
  // or exp(x) ~ 1 + 2x * A * [(2 - x)*A + x^2(140 + x^2)] (eliminating the second ’/’ using property K/(P / Q) == QK/P)                           (2)
  // where A in (1) or (2) = 840 + 20x^2 (a repeating term in the numerator and denominator)
  return 1. + 2. * x * a / ((2. - x) * a + x2 * (140. + x2)); // converges faster than taylor’s
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
  // And log(M) == ln(M) / ln(10):
  // The contribution ln(M) to the exponent is approximated using taylor’s series about point b {b == SQRT(2)}

  // Let y = log(f)
  // The Taylor approx. is given as log(b) + plog(2) + df.dy/df - d2f.d2y/df2
  dF = bits.F - Precalc_SQRT2;

  p10 = Precalc_LogSQRT2b10 + ((p2 * Precalc_Log2b10) + ((dF * Precalc_DydFLogb10_f) - (dF * (dF * Precalc_D2ydF2Logb10f_Div2))));

  // Reduce F to [1, 2)
  // That is, F has to be multiplied by 10^-p such that F  1 and == f and f x 10^p == F
  p2  = Precalc_Log10b2 * p10; // log2(p10)
  // Split 10^p into 2^-p2 * 10dp [dp = p10 - p2]
  // 10 ^ dp is approximated using e^Ln(dp)
  dp     = (p10 * Precalc_Loge10) - (p2 * Precalc_Loge2);

  bits.N = (1023ULL + p2) << 52; // 2^-p2

#ifdef MAX_PREC_CALC_FLT
  // reduce dp closer to 0 for better approximation of exp
  dp = .1 * dp;
#endif

  dp = exp__(dp); // e^Ln(dp) ~ 10^(p10 - p2)

#ifdef MAX_PREC_CALC_FLT
  // correct reduction
  dp = xpow10__(dp);
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


int main(void) {
  bits_t k;
  k.F = DBL_MIN;

  putf(reprexp(k));
  putf(DBL_MIN);
}
