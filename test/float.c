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
#define PASS(...) (void *)0
#define expOf(...) true

#if defined(__GNUC__) || defined(__clang__)
    #define FORCEI_PURE __attribute__((pure, always_inline))
#elif defined(_MVSC) || defined(_WIN32)
    #define FORCEI_PURE __forceinline
#else
    #define FORCEI_PURE
#endif
#define INLINE(T) __inline__ FORCEI_PURE T

#if defined(__GNUC__) || defined(__clang__) || ((-16 >> 1) == -8 && ((-1 & (16 - 1)) == 15))
// Check if compiler supports correct bit operations on signed bits (according to the standard, this is implementation defined)
// Test is still untrusted since CPP may implemented by a different implementator from that of the actual compiler, but it is unlikely
    #define COMPILER_SUPPORT_SIGNED_BIT_OP 1
#else
    #define COMPILER_SUPPORT_SIGNED_BIT_OP 0
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define LIKELY___(x, p) __builtin_expect(x, p)
#else
    #define LIKELY___(...)
#endif

// We require an integer of bit-width W, wide enough to accomodate the underlying bit-width of float or double

#if !defined(UINT64_MAX) || defined(USE_FLOAT32__)
    #define DISABLE_DBL_SUPPORT__
    #ifndef UINT32_MAX
        #error "No float32 or float64 support"
    #endif
    #define UINT_ uint32_t
    #define INT_  int32_t
#else
    #define UINT_ uint64_t
    #define INT_  int64_t
#endif

// Fallback to float32: for systems without double (rarely, but some embedded system); Manually disabling use of double by defining the macro USE_FLOAT32__
#if (defined(FLT_DIG) && defined(DBL_DIG) && (DBL_DIG == FLT_DIG)) || defined(DISABLE_DBL_SUPPORT__)
    #define DBLT__      float
    #define DFLT_BIAS__ 127ul
    #define DBLT_MANT_SHFT   23
    #define Precalc_AllOnesMantBits 0x3ffffful
#else
    #define DBLT__      double
    #define DFLT_BIAS__ 1023ull
    #define DBLT_MANT_SHFT   52
    #define Precalc_AllOnesMantBits 0xfffffffffffffull
#endif

#define MAX_PREC_CALC_FLT 1


typedef union {
  DBLT__  F;
  UINT_   N;
} bits_t;

typedef struct {
  bits_t  fmt_Num;
  int16_t fmt_Exp;
  int16_t fmt_Err;
} fmt;

void   reprfloat   (bits_t bits);
DBLT__ reprexp     (bits_t bits);
DBLT__ approxLog2  (bits_t bits);
DBLT__ approxLog10 (bits_t bits);
DBLT__ xpow10__    (const DBLT__ x);
DBLT__ exp___      (const DBLT__ x);
DBLT__ exp__       (const DBLT__ x);

static const DBLT__ Precalc_2powDdivP[16] =
  {
   // 2^d/p or p_root(2^d); P = 16
   1.00000000000000000, 1.0442737824274138, 1.0905077326652577,
   1.13878863475669160, 1.1892071150027210, 1.2418578120734840,
   1.29683955465100960, 1.3542555469368927, 1.4142135623730951,
   1.47682614593949930, 1.5422108254079407, 1.6104903319492543,
   1.68179283050742900, 1.7562521603732995, 1.8340080864093424,
   1.91520656139714740
  };

#define Precalc_Log10b2  3.3219280948873620
#define Precalc_Log2b10  0.3010299956639812
#define Precalc_Loge2    0.6931471805599453
#define Precalc_InvLoge2 1.4426950408889630
#define Precalc_Loge10   2.3025850929940460

#define Precalc_1_f_asInt    0x3ff0000000000000ull
#define Precalc_FloatExpBias 0x10000000000000ull
#define Precalc_53shift52    0x350000000000000ull

#define Precalc_SQRT2              1.4142135623730951
#define Precalc_LogSQRT2b10        0.1505149978319906
#define Precalc_DydfLogSQRT2_b10   0.3070925731856877
#define Precalc_D2ydf2LogSQRT2_b10_Div2 0.108573620475813

#define Precalc_16DivLoge2 23.083120654223414
#define Precalc_Loge2Div16 4.3321698784997e-2

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

extern INLINE(DBLT__) approxLog2(bits_t bits) {
  bits_t FI;

  /*
   * Approximate log10(f) {0 > f <= 2^p-1}
   * We exploit the logarithmic nature of float’s bit pattern (IEEE) such AB * CD (AB=exponent bits; CD=mantissa bits) scaled down to f / 2^p
   * (where p is the precision of the mantissa of f) is an approximate of log2(f)
   */
  // Second iteration requires F = F * c {c: 0.6666} (FLoating Point Tricks By Blinn), however F * x may reduce to F < 0. Since this is below our limit, it is trouble. However, we can perform F * 2c, relying on the fact that LogN(F) = LogN(F * N) - 1 {F: F => 1} or LogN(F / N) - 1 {F: F < 1}
  // So Log2(F * c * 2) ~ 1 - log2(F * C) (Keeping F above 0)
  FI.F = bits.F * 1.3332;
  //Subtract bias (DBLT__=1023) from exponent (without shifts)

  bits.F = bits.N - Precalc_1_f_asInt; //0x4000000000000000ull;
  // Subtract 52 from the exponent which effectively divides by 2^52 directly
  bits.N = bits.N - Precalc_53shift52;

  FI.F = FI.N - Precalc_1_f_asInt;
  FI.N = FI.N - Precalc_53shift52;

  return bits.F + (FI.F - 0.156);
}

extern INLINE(DBLT__) approxLog10(bits_t bits) {
  // logaX ~ logbX/logbA or logbX * 1/logbA or 1/logxA

  return (approxLog2(bits) * Precalc_Log2b10); // return log2(f) / log10(2)
}

extern inline void reprfloat(bits_t bits) {
  PASS();
}


extern INLINE(DBLT__) xpow10__(const DBLT__ x) {
  register DBLT__ x2, x4, x8;

  // Calculate x ^ 10
  x2 = x  * x;
  x4 = x2 * x2;
  x8 = x4 * x4;

  return x8 * x2;
}


extern INLINE(DBLT__) exp__(const DBLT__ x) {
  /* Calculate e^x for x in interval [0, 1);

   * We first reduce x (make it smaller) and take its exponential
   * Let:
   * Q, q = qoutient (truncated)
   * R, r = remainder
   * d    = divisor
   *
   * If   Q == x/d, R == x - Q*d
   * Then x == Q*d + R
   * We use this to split x into a qoutient Q (may be large) and a small remainder, R.
   *
   * Taking the exponential of x:
   * e^x = e^(Q*d + R) OR e^x = e^(Q*d) * e^R
   *
   * If d is choosen to be a rational number, with Ln(k) as the dividend (where k is any positive integer---preferably), and p as the divisor, then d may be wriiten as:
   *   d = Ln(k) / p         (p > 0)
   *
   * Putting d into e^x:

   * e^x = e^(Q * Ln(k)/p) * e^R ==== e^(Q/pLn(k)) * e^R ==== e^Ln(k^(Q/p)) * e^R
   * Therefore, e^x == k^(Q/p) * e^R
   *
   */

  // If K = 2, P = 16, then d = Ln(2)/16 and Q = x/d = x / (Ln2/16) or Q = 16x/Ln(2)
   const INT_ Q = x * Precalc_16DivLoge2;

   /* Calculate 2^Q/p, where:
	    2^Q/p == 2^q * 2^r (q = Qoutient(Q/p)...),
    *       r == Q mod p (as integer) OR r == (Q mod p) / p (as decimal),
    *       2^r == p_root(2^(Q mod p)) and (Q mod p) < p
    *
    * 2^r or p_root(2^(Q mod p)) is calculated using a small lookup table of size p
    */
   bits_t p2;

#if COMPILER_SUPPORT_SIGNED_BIT_OP == 0
   // Implementation defined behaviour of bitwise OP on negative numbers. Some implementators provide expected results. Rather not, we handle negative Q specially

   if (LIKELY___((Q < 0), 0)) {
     p2.F = Precalc_2powDdivP[16 - (-Q & 0xf)];  // -N mod p == 16 - (N mod 16)  // N mod 16 =~ N & (16 - 1)
     p2.N -= ((-Q >> 4) + 1) << DBLT_MANT_SHFT;

     goto exp;
   }
#endif
   // Q >= 0 | bit operations are guaranteed to be valid on signed negative integers
   puti(Q & 0xf);
   p2.F = Precalc_2powDdivP[Q & 0xf];
   p2.N += (Q >> 4) << DBLT_MANT_SHFT; // Safe, since Q>>4 is always small (< 5)

 exp:

   // (2^q * 2^r) * e^R ==== 2^(Q/d) * e^R ==== e^x
   return  p2.F * exp___(x - (Q * Precalc_Loge2Div16));
}
extern INLINE(DBLT__) exp___(const DBLT__ x) {
  /* By continued fraction,
     e^x = 1 + 2x / (2 - x + x^2 / (6 + x^2 / (10 + x^2 / 14 + ...)))
     =~ 1 + 2x(840 + 20x^2) / [(2 - x)(840 + 20x^2) + (140 + x^2)x^2]  (5 terms only)
*/
  const DBLT__ x2 = x * x, a = 840. + 20. * x2; //Let A = 840 + 20x^2

  // exp(x) =~ 1 + 2x * A * [(2 - x)*A + x^2(140 + x^2)]  (simplified the C.F)
  return 1. + 2. * x * a / ((2. - x) * a + x2 * (140. + x2));
}

extern INLINE(DBLT__) reprexp(bits_t bits) {
  int16_t p2, p10, p2tmp;
  DBLT__  F,  dF,  dp, dp2;

  F = bits.F; // Float value

  //  Exponent
  p2tmp = p2 = (bits.N >> DBLT_MANT_SHFT) - DFLT_BIAS__;
  puti(p2);

  // Normalized Mantissa. Within [1, 2)
  bits.N = (bits.N & Precalc_AllOnesMantBits) | (DFLT_BIAS__ << DBLT_MANT_SHFT);

  /* The exponent of F is Approximated using log10(f) where f ~ 2^p * M
   * log(F) ~ log(2^p * M) ~ plog(2) + log(M)
   * And log(M) == ln(M) / ln(10):
   * The contribution log(M) to the exponent is approximated using taylor’s series of Log(M) about point b (where b == SQRT(2))

   * Let y = log(f)
   * The Taylor approx. is given as log(b) + dF*dy/df - dF^2*(1/2*d2y/df2)...
   */
   dF = bits.F - Precalc_SQRT2; // f - a

  p10 = (p2 * Precalc_Log2b10) + (Precalc_LogSQRT2b10 + ((dF * Precalc_DydfLogSQRT2_b10) - (dF * (dF * Precalc_D2ydf2LogSQRT2_b10_Div2))));

  /* Reduce F such that floor(abs(F)) < 10
   * That is, We divide F by 10^p (F ~ N1N2N3... becomes F ~ N1.N2N3...)
   *
   * 10^p is spltted into 2^p2 * 10^dp (where: 2^p2 <= 10^p < 2^(p2+1))
   *
   * p2 = trunc( log2(p) )
   * dp = p - p2
   */
  bits.F = F;
  p2  = (Precalc_Log10b2 * p10); // p2 = log2(p)
  bits.N = (bits.N & Precalc_AllOnesMantBits) | ((DFLT_BIAS__ - p2 + ptmp) << DBLT_MANT_SHFT); // F / 2^p2

  // We take the natural logarithmic difference, so that 10^dp ~ e^Ln(dp)
  // making it an ease to calculate, since dp is small and e^dp has good and cheap approximation techniques for small numbers (< 1)
  dp  = (p10 * Precalc_Loge10) - (p2 * Precalc_Loge2); // p - p2
  dp = exp__(dp); // e^Ln(dp) ~ 10^(p10 - p2)

  return 5e-14 + (bits.F / dp); // (F / 2^p) / 10^dp
}

static inline void floatRepr(DBLT__ f) {
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
  k.F = 2342344534.;

  putf(reprexp(k));
  putf(k.F);
}
