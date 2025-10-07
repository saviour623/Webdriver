/* MVPG API: MISC
Copyright (C) 2025 Michael Saviour

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#define V_MISC_VISIBLE
#include "v_misc.h"

#define Precalc_Log10b2  3.3219280948873620
#define Precalc_Log2b10  0.3010299956639812
#define Precalc_Loge2    0.6931471805599453
#define Precalc_InvLoge2 1.4426950408889630
#define Precalc_Loge10   2.3025850929940460

#define Precalc_SQRT2              1.4142135623730951
#define Precalc_LogSQRT2b10        0.1505149978319906
#define Precalc_DydfLogSQRT2_b10   0.3070925731856877
#define Precalc_D2ydf2LogSQRT2_b10_Div2 0.108573620475813

#define Precalc_16DivLoge2 23.083120654223414
#define Precalc_Loge2Div16 4.3321698784997e-2

typedef uint16_t mask_t;
typedef struct  {
  DBLT__ f_val;
  uint16_t f_digits;
} Dtoa_expInfo;

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

     JMP_(Exp);
   }
#endif
   // Q >= 0 | bit operations are guaranteed to be valid on signed negative integers
   puti(Q & 0xf);
   p2.F = Precalc_2powDdivP[Q & 0xf];
   p2.N += (Q >> 4) << DBLT_MANT_SHFT; // Safe, since Q>>4 is always small (< 5)

   LOCATION(Exp);

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

extern INLINE(DBLT__) Dtoa_exp(bits_t bits) {
  int16_t p2, p10, p2tmp;
  DBLT__  F,  dF,  dp, dp2;

  F = bits.F; // Float value

  //  Exponent
  p2tmp = p2 = (bits.N >> DBLT_MANT_SHFT) - DFLT_BIAS__;
  putd(p2);

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

  p10 = (p2 * Precalc_Log2b10) + (Precalc_LogSQRT2b10 + ((dF * Precalc_DydfLogSQRT2_b10) - (dF * (dF * Precalc_D2ydf2LogSQRT2_b10_Div2)))) - 2;

  /* Reduce F such that floor(abs(F)) < 10
   * That is, We divide F by 10^p (F ~ N1N2N3... becomes F ~ N1.N2N3...)
   *
   * 10^p is spltted into 2^p2 * dp
   * where:  p2 = trunc( log2(p); 2^p2 <= 10^p < 2^(p2+1)); dp = 10^p / 2^p2
   * 2^p2 * dp == 10^p
   */
  bits.F = F;
  p2  = (Precalc_Log10b2 * p10); // p2 = log2(p)
  bits.N = (bits.N & Precalc_AllOnesMantBits) | ((DFLT_BIAS__ - p2 + p2tmp) << DBLT_MANT_SHFT); // F / 2^p2

  /*
     We can calculate dp by taking the natural logarithm of dp and approximating e^dp
     * ln(dp) = ln(10^p / 2^p2) = ln(10^p) - ln(2^p2) ~ pln(10) - p2ln(2)
     */
  dp  = (p10 * Precalc_Loge10) - (p2 * Precalc_Loge2); // p - p2
  dp = exp__(dp); // e^Ln(dp) ~ 10^(p10 - p2)

  return 5e-15 + (bits.F / dp); // (F / 2^p) / 10^dp
}

vsize_t Dtoa(DBLT__ f, Pp_Setup *setup) {
  char *bf;

  if (f == .0) {
  }
}

extern INLINE(vsize_t) hex(uintmax_t n, char *bf, _Bool prefix);
extern INLINE(vsize_t) oct(uintmax_t n, char *bf, _Bool prefix);
extern INLINE(vsize_t) dec(uintmax_t n, char *bf, _Bool prefix __UNUSED__);

static __inline__ vsize_t Itoa(uintmax_t n, Pp_Setup *setup) __ {
  mask_t mask = setup->mask;
  uint8_t isneg __attribute__((unused));

  if ( !(mask & USIGNED) ) {
	if ((isneg = (intmax_t)n < 0)){
	  n = -(intmax_t)n;
	  setup->bf++ = '-';
	}
  }
  return (mask & OCT ? oct
		  : mask & HEX ? hex : dec)(n, setup->bf) + isneg;
}

#define IS_INT(mask) (mask & 0b111100000u)
#define IS_FLOAT(mask) (mask & 0b11000000000u)
#define IS_CHARS(mask) (mask & 0b1100000000000u)
#define IS_ADDRS(mask) (mask & 0b10000000000000u)

#define clrOldMask(c, mask, clrMaskArray)		\
  (mask & clrMaskArray[(c = mapChar[c]) >> 5])

#define setNewMask(c, cc)							\
  (setToshftPos(c) << shft1_IfAllowRepeat(c, cc))

#define setToshftPos(c)							\
  ((uint16_t)(!!c) << (c & 0b1111))

#define shft1_IfAllowRepeat(c, cc)				\
  (!(cc ^ c) & !!(c & 0b10000))

enum CLR {
		  CLR_ALL = 0,
		  CLR_KEEP_HASH = 0b10000u,
		  CLR_KEEP_HLL   = 0b1101u,
		  CLR_KEEP_HLL_HASH = 0b11101u,
};

enum {
	  HEX = 0b10000000u,
	  OCT = 0b100000000u,
	  USIGNED = 0b100000u,
	  INTTYPE = 0b100000000001101u,
	  SHRT = 0b1,
	  LNG = 0b100u,
	  LLNG = 0b1000u,
	  PTR = 0b100000000000000u
}

/*
  Map-Char-Mask: 0b 11 (clrMaskIndex) 1 (AllowRepeat) 1111 (No. of shift)
 */
static const uint8_t mapChar[127] =				\
  {
   ['h'] = 0b110000,  ['l'] = 0b110010,  ['L'] = 0b110011,  ['q'] = 0b110011,
   ['#'] = 0b100,     ['i'] = 0b1000101, ['d'] = 0b1000101, ['u'] = 0b1000110,
   ['x'] = 0b1100111, ['o'] = 0b1101000, ['f'] = 0b1001001, ['e'] = 0b1001010,
   ['c'] = 0b1011,    ['s'] = 0b1100,    ['p'] = 0b1101
  };

static inline __attribute__((always_inline)) uint16_t formatMask(const char *fmt) {
  const uint16_t clrMask[4] = {CLR_ALL, CLR_KEEP_HASH, CLR_KEEP_HLL, CLR_KEEP_HLL_HASH};
  register uint16_t mask = 0;
  register uint8_t  c = 0, cc = 0;

  while ((c = *fmt++)) {
	mask = clrOldMask(c, mask, clrMask) | setNewMask(c, cc);
	cc = c; //keep old value
  }
  return mask;
}

__NONNULL__ vsize_t VEC_Repr(void *v, Pp_Setup *setup) {
  vsize_t size;
  uint16_t mask;

  if (setup->fmt == NULL)
	return 0;

  if ((mask = formatMask(setup->fmt)) == 0) {

	return 0;
  };

#if defined(PRINT_MAX) && (PRINT_MAX > 0)
  size = setup->size < PRINT_MAX ? setup->size : PRINT_MAX;
#else
  size = setup->size;
#endif
  setup->mask = mask;

  if (IS_INT(mask)) {
	JMP(isint);
	LOCATION(IS_INT);
	if (minmax(0, size / (double)intMaxDigit(mask)), 1) {
	  switch (mask & INTTYPE) {
	  case SHRT:
		VEC_map(v, Itoa, ushort, setup->bf, mask & USIGNED);
		break;
	  case LNG:
		VEC_map(v, Itoa, ulong, setup->bf, mask & USIGNED);
		break;
	  case LLNG:
		VEC_map(v, Itoa, ulongLong, setup->bf, mask & USIGNED);
		break;
	  case PTR:
		for (uintmax_t po = 0 ; po < size; po++) {
		  Itoa((uintmax_t)(v + po), setup);
		}
		break;
	  default:
	    VEC_map(v, Itoa, int);
	  }
	}
  }

  if (IS_FLOAT(mask))
    ;

  if (IS_CHARS(mask))
    ;

  if (IS_ADDRS(mask)) {
	mask |= 0;
	JMP(JMP_ADDRS);
  }
}

#endif
