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
typedef union {
  DBLT__ F;
  UINT_  N;
}  Dtoa_IEEEFloat;

typedef struct  {
  DBLT__ expval;
  uint16_t expdig;
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
   *
   * For better accuracy, we find a way to reduce x even smaller, say within [0, 0.1). We know that for any polynomial x, x = Q*d + R (where: Q->Qoutient, d->Divisor, R->Remainder).
   *
   * If x -> Q*d + R, then taking the exponential of x:
   * e^x -> e^(Q*d + R) OR e^x -> e^(Q*d) * e^R
   *
   * If we choose d carefully to be a rational fraction involving Ln(N), then d -> Ln(N) / p
   * N and p is any positive number (N > 0, p > 0).
   *
   * Putting d into e^x:
   * e^x -> e^(Q * Ln(N)/p) * e^R == e^(Q/p * Ln(N)) * e^R == e^Ln(N^(Q/p)) * e^R == N^(Q/p) * e^R
   *
   * So we get that e^x -> N^(Q/p) * e^R (where: R << x)
   *
   * We choose N -> 2 and p -> 16 so that e^x -> 2^(Q/16) * e^R
   *
   */

  const INT_ Q = x * Precalc_16DivLoge2; //x * (1/d)

   /* Calculate 2^Q/16, where:
	    2^Q/16 -> 2^q * 2^r (q -> Qoutient(Q/16), r -> (Q mod 16)/16 (as float),
    *       2^r == 16_root(2^(Q mod p))
    *
    * 2^r or 16_root(2^(Q mod 16)) can be calculated using a small lookup table of size 16 since (Q mod 16) < 16
    */
  Dtoa_IEEEFloat p2;

#if !COMPILER_SUPPORT_SIGNED_BIT_OP
   if (Q < 0) {
     p2.F = Precalc_2powDdivP[16 - (-Q & 0xf)];  //-Q mod p == 16 - (Q mod 16)
     p2.N -= ((-Q >> 4) + 1) << DBLT_MANT_SHFT; // 2^q * 2^r

     JMP_(Exp);
   }
#endif
   // Q >= 0 or bit operations are guaranteed to be valid on negative integers
   p2.F = Precalc_2powDdivP[Q & 0xf];
   p2.N += (Q >> 4) << DBLT_MANT_SHFT;

   LOCATION(Exp);

   // e^x == 2^(Q/d) * e^R == (2^q * 2^r) * e^R
   return  p2.F * exp___(x - (Q * Precalc_Loge2Div16));
}

extern INLINE(DBLT__) exp___(const DBLT__ x) {
  /* By continued fraction,
     e^x = 1 + 2x / (2 - x + x^2 / (6 + x^2 / (10 + x^2 / 14 + ...)))
     =~ 1 + 2x(840 + 20x^2) / [(2 - x)(840 + 20x^2) + (140 + x^2)x^2]  (5 terms only)
*/
  const DBLT__ x2 = x * x, a = 840. + 20. * x2; //Let a = 840 + 20x^2

  return 1. + 2. * x * a / ((2. - x) * a + x2 * (140. + x2));
}

extern INLINE(void) Dtoa_Normal(Dtoa_IEEEFloat IE3Bit, Dtoa_expInfo *Finfo) {
  int16_t p2, p2tmp, p; //log
  DBLT__  F,  dF,  dp, dp2;

  F = IE3Bit.F;
  p2tmp = p2 = (IE3Bit.N >> DBLT_MANT_SHFT) - DFLT_BIAS__; // effective log2(f)

  // set mantissa within [1, 2)
  IE3Bit.N = (IE3Bit.N & Precalc_AllOnesMantBits) | (DFLT_BIAS__ << DBLT_MANT_SHFT);


  // Find Log(f): GIven that f ~ 2^p2 * M then Log(f) ~ Log(2^p2 * M) ~ p2*Log(2) + Log(M)
  //  Let y = Log(M), we approximate Log(M) using taylor series about point b: Log(b) + dF*dy/df - dF^2*(1/2*d2y/df2)...
  dF = IE3Bit.F - Precalc_SQRT2; // f - b
  p = (p2 * Precalc_Log2b10) + (Precalc_LogSQRT2b10 + ((dF * Precalc_DydfLogSQRT2_b10) - (dF * dF * Precalc_D2ydf2LogSQRT2_b10_Div2))) - 2; // [ Log(f) or log10(f) ] - 2

  // divide f by 10^p: to avoid over/under-flow, 10^p is splitted to 2^p2 * dp (dp -> 10^p - 2^p)
  IE3Bit.F = F;
  p2  = (Precalc_Log10b2 * p); // p2 = log2(p)

  /*
     dp ~ 10^p - 2^p can be approximated using log and exp function:
     * if Ln(dp) -> Ln(10^p / 2^p2) -> Ln(10^p) - Ln(2^p2) ~ p*Ln(10) - p2*Ln(2)
	 * then dp = e^(p*ln(10) - p2ln(2))
     */
  dp  = (p * Precalc_Loge10) - (p2 * Precalc_Loge2); // p - p2
  dp = exp__(dp); // e^Ln(dp) ~ 10^(p10 - p2)

  IE3Bit.N = (IE3Bit.N & Precalc_AllOnesMantBits) | ((DFLT_BIAS__ - p2 + p2tmp) << DBLT_MANT_SHFT); // F / 2^p2
  IE3Bit.F = IE3Bit.F / dp; // (F / 2^p) / 10^dp ~ F / 10^p

  Finfo->expval = IE3Bit.F;
  Finfo->expdig = p;
}

const uint32_t DDtoa(double f, char *bf, const int prec) {
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

vsize_t Dtoa(DBLT__ f, Pp_Setup *setup) {
  Dtoa_expInfo FInfo = {0};
  Dtoa_IEEEFloat   Bit = {.F = f};

  if (f == .0) {
	return appendPrefix(ZeroFloat, setup->bf);
  }

  if (IS_NAN(Bit.N)) {
	return appendPrefix(Nan, setup->bf);
  }

  if (IS_INF(Bit.N)) {
	return appendPrefix(Inf, setup->bf);
  }

  if (minmax(SMALL_FLOAT, f, BIG_FLOAT)) {
	Dtoa_Normal(Bit, &FInfo);
  }
}

extern INLINE(vsize_t) hex(uintmax_t n, char *bf, _Bool prefix);
extern INLINE(vsize_t) oct(uintmax_t n, char *bf, _Bool prefix);
extern INLINE(vsize_t) dec(uintmax_t n, char *bf, _Bool prefix __UNUSED__);

static INLINE(vsize_t) Itoa(uintmax_t n, Pp_Setup *setup) {
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

const INLINE(uintmax_t) valInt(const void *u, const mask_t mask) {
  return mask & LNG ? *(long *)(u) : mask & LLNG ? *(long long *)(u)
	: mask & SHRT ? *(short *)(u) : *(int *)(u);
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

  if ((setup->fmt == NULL) || (mask = formatMask(setup->fmt)) == 0) {
	return 0;
  };

#if defined(PRINT_MAX) && (PRINT_MAX > 0)
  size = setup->size < PRINT_MAX ? setup->size : PRINT_MAX;
#else
  size = setup->size;
#endif
  setup->mask = mask;

  if (IS_INT(mask)) {
	LOCATION(IS_INT);
	if (minmax(0, size / (double)intMaxDigit(mask)), 1) {
	  for (int oo = 0; oo < size; oo++) {
		Itoa(valInt(v+oo, mask), setup->bf, mask & USIGNED);
		appendPrefix(sep, setup->bf);
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
