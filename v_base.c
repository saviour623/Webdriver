#include "v_base.h"

#ifdef VEC_INTERNAL_CCS

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

#define Precalc_SQRT2              1.4142135623730951
#define Precalc_LogSQRT2b10        0.1505149978319906
#define Precalc_DydfLogSQRT2_b10   0.3070925731856877
#define Precalc_D2ydf2LogSQRT2_b10_Div2 0.108573620475813

#define Precalc_16DivLoge2 23.083120654223414
#define Precalc_Loge2Div16 4.3321698784997e-2

#define DOCUMENTATION(LABEL) LABEL
#define VEC_BUFFER_SIZE USHRT_MAX
#define VEC_MAX_INT_LEN 32
#define EOFMT(c, f)         (((c) = (f)) & ((c) ^ 58))
#define COMMA(bf) ((*(bf)++=','), (*(bf)++=' '), 2)

enum {
      PTR  = 0x01,  USIGNED = 0x02,  CHAR  = 0x04,
      LONG = 0x100, LLNG    = 0x200, SIZE = 0x800,
      TYPE = 0x7f,  SPEC    = 0xf00, BASE = 0x400
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

static inline void floatRepr(DBLT__ f) {
  bits_t bitsv;

  bitsv.F = f;
  // absolute(f)
  if (f < 0.0)
    f = -f;
  // Check for -0 or +0
  PASS;
  // check for inf
  PASS;
  // check for nan
  PASS;
}

__NONNULL__ __STATIC_FORCE_INLINE_F void VEC_Itoa(const intmax_t n, Pp_Setup *cf) {
  int U = cf->Pp_used;

  if ((cf->Pp_size - U) < cf->Pp_overflw)
    return;
  U = COMMA(cf->Pp_buf);
  U += MvpgInclude_Itoa(n, cf->Pp_buf, !!(cf->Pp_mask & BASE), !(cf->Pp_mask & USIGNED) && (n < 0));
  cf->Pp_used = U;
}

__NONNULL__ void VEC_TostrInt(void *v, Pp_Setup *cf) {
  char *saveBuf;

  if (cf->Pp_mask & PTR) {
    //VEC_map(v, VEC_Itoa, uintptr_t, cf, true);
    cf->Pp_overflw = VEC_widthOfCInt[sizeof(uintptr_t)>>2] + 2;
    goto end;
  }
  switch (cf->Pp_mask & SPEC) {
  case LONG:
    cf->Pp_overflw = VEC_widthOfCInt[sizeof(long)>>2] + 2;
    VEC_map(v, VEC_Itoa, long, cf);
    goto end;
  case LLNG:
    cf->Pp_overflw = VEC_widthOfCInt[sizeof(long long)>>2] + 2;
    VEC_map(v, VEC_Itoa, long long, cf);
    goto end;
  case SIZE:
    cf->Pp_overflw = VEC_widthOfCInt[sizeof(size_t)>>2] + 2;
    VEC_map(v, VEC_Itoa, size_t, cf);
    goto end;
  default:
    cf->Pp_overflw = VEC_widthOfCInt[sizeof(int)>>2] + 2;
    VEC_map(v, VEC_Itoa, int, cf);
  }
 end:
  PASS;
}

/*                    REPR (POSSIBLE REPRESENTATION OF VECTOR)
 *
 * Pp_Setup.fmt -> Format
 * ---------------------

 * Use: .fmt = "d:^.5"

   TYPE SPECIFIERS
 * h   - unsigned short
 * hh  - signed short
 * u   - unsigned int
 * i   - signed int
 * d   - signed int
 * q   - signed long long
 * z   - size_t
 * w   - wide char (unimplemented)
 * l1  - int8_t
 * l2  - int16_t
 * l4  - int32_t
 * l8  - int64_t
 * lz  - int128_t
 * f   - float
 * c   - unsigned char
 * c   - signed char
 * s   - char array
 * p   - pointer

 * WIDTH & SIGNESS SPECIFIERS
 * l - long int / long float (double)
 * ll - long long int (if supported) / long double
 * h  - signed

 * OUTPUT SPECIFIERS / OTHERS
 * ^ - Uppercase Hexadecimal
 * # - Append "0x" to hexadecimal
 * *
 * +
 * -
 * .N...
 *
   NOT SUPPORTED
 * Uppercase type/width specifiers (F, L, ...)
 * Paddings (not necessary)

   NOTE
 * Underlying type of Type Specifier T, must be the same size as VEC_sizeof(Vector).
 * Floats are respected and not promoted to doubles.
 */

__NONNULL__ __STATIC_FORCE_INLINE_F void VEC_TostrFlt(const void *v, Pp_Setup *cf) {
  switch (cf->Pp_mask & SPEC) {
  case LONG:
  case LLNG:
  default:
    PASS;
  }
  VEC_assert(false, "Repr: TOSTRFLT: UINMPLEMENTED");
}

__NONNULL__ vsize_t VEC_Repr(void *v, Pp_Setup *setup) {
  register uint32_t mask;
  register uint8_t  c;

  /* It may be desirable to skip the format processing (Usually on a second... call toRepr with the same format) */
  if (setup->Pp_skip && (mask=setup->Pp_mask))
    goto TypeCheckNAction;

  {
    uint8_t mskc, error, *fmt, fc[32] = {0};

    fmt = setup->Pp_fmt;
    VEC_assert(EOFMT(c, *fmt++), "Repr: Empty Format is unsupported");

    DOCUMENTATION (CopyFormatToFc):

    fc[0] = c;
    for (mskc = 0; (mskc < 6) && EOFMT(c, *fmt); mskc++)
      fc[(1u << mskc) - 1] = c;

    DOCUMENTATION (GetFormatInExpectedOrder):

    mask = fc[0] == 0x6c;
    mask = (mask << (c=fc[mask] == 0x6c)) | c;
    mask = (mask << (c=fc[mask] == 0x7a)) | c;
    mask = (mask << (c=fc[mask] == 0x78)) | c;
    mask = (mask << (c=fc[mask] == 0x68)) | c;
    mask = (mask << 8) | fc[mask];

    DOCUMENTATION (CheckIgnoredSPecOrFormat):

    #define IgnoredMaskOrFormat(M, L, F, C)		\
    (!((M) >> ((L)+6)) || EOFMT(C, F))

    DOCUMENTATION (ErrorCheck):

    error = !(
	      (((mask & TYPE) == 0x73) && (mask & SPEC))
	    | (((mask & TYPE) == 0x66) && (mask & BASE))
	    | IgnoredMaskOrFormat(mask, mskc, *fmt, c)
	    );
    VEC_assert(error, "Repr: Invalid Format");

    #undef IgnoredMaskOrFormat

    setup->Pp_fmt = fmt;
  }

 TypeCheckNAction:
  setup->Pp_dtype = VEC_vdtype(v);
  setup->Pp_mask  = mask & SPEC; /* Igore Type bits (Reused for other mask) */
  switch ( mask & TYPE ) {
  case 'p':
    setup->Pp_mask |= PTR;
  case 'u':
    setup->Pp_mask |= USIGNED;
  case 'd': case 'i':
    VEC_TostrInt(v, setup);
    break;
  case 'f':
    VEC_TostrFlt(v, setup);
    break;
  case 'c':
    setup->Pp_mask |= CHAR;
  case 's':
    VEC_assert(
	       (setup->Pp_dtype > 1) && (setup->Pp_dtype != sizeof(VEC_type(char))),
	       "Repr: Type Mismatch"
	       );

    if ( setup->Pp_used > 0 ) {
      char *Vv, *bf;
      register vsize_t bfs = setup->Pp_size, e = VEC_vused(v), i = 0;

      Vv = setup->Pp_mask & CHAR ? (i=e-1), v : VEC_typeCast(v, char *)[0];
      bf = setup->Pp_buf;
      do {
	/* Iteratively Copy strings from Object v, to bf. bf is updated to the last written point for next copy */
        bfs -= MvpgInclude_strlcpy(&bf, Vv, bfs);
	Vv = VEC_typeCast(v, char *)[++i];
      } while( (i < e) && bfs );
    }
  default :
    /* TODO: Raise Error */
    setup->Pp_used = 0;
  }

  setup->Pp_mask |= mask & TYPE; /* restore Type */
  return setup->Pp_used;
}

#endif
