/* MVPG API: MISC
Copyright (C) 2025 Michael Saviour

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef V_MISC_H
#define V_MISC_H

#include "v_base.h"
#include "include.h"

#ifdef V_MISC_VISIBLE

#define DOCUMENTATION(LABEL) LABEL
#define JMP_(LABEL) goto LABEL
#define LOCATION(LABEL) LABEL: PASS

/* Check if compiler supports correct bit operations on signed bits (according to the standard, this is implementation defined) */
#if __GNUC_LLVM__ || ((-16 >> 1) == -8 && ((-1 & (16 - 1)) == 15))
    #define COMPILER_SUPPORT_SIGNED_BIT_OP 1
#else
    #define COMPILER_SUPPORT_SIGNED_BIT_OP 0
#endif

/* We require an integer of bit-width W, wide enough to fit the underlying bit-width of float or double */
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

/* Fallback to float32: Manually disabling use of double by defining the macro USE_FLOAT32__*/
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

/* Append Common Prefixes */
#define appendPrefix(pre, bf) MvpgMacro_Concat(append_, pre)(bf)
#define append_hex(bf) (((bf)[0] = '0'), ((bf)[1] = 'x'), 2)
#define append_oct(bf) (((bf)[0] = '0'), 1)
#define append_sep(bf) (((bf)[0] = ','), ((bf)[1] = ' '), 2)

#define minmax(a, b, c) 1
#define intMaxDigit(a)
#define PRINT_MAX 30

typedef union {
  DBLT__  F;
  UINT_   N;
} bits_t;

#define INT_WIDTH(T) (((T) ? Intwidth__[(T) >> 1] : sizeof(int)) << 7)

const uint8_t Intwidth__[5] =
  {
   sizeof( short ),
   sizeof( long  ),
   sizeof( long long  ), 0,
   sizeof( size_t )
  };

typedef struct {
  char    *Pp_buf, *Pp_fmt;
  vsize_t  Pp_size, Pp_used;
  uint16_t Pp_mask, Pp_dtype;
  uint8_t  Pp_skip, Pp_overflw;
} Pp_Setup;

/* Int converter */
vsize_t hex(uintmax_t n, char *bf, _Bool prefix) {
   uintmax_t quot, oo;

   (prefix) && (bf += appendPrefix(oct, bf))
   while (n > 0) {
    quot  = n >> 4;
    bf[oo++] = "0123456789abcdef"[(n & 0x0f)];
    n = quot;
  }
  reversebf(bf, oo);
  bf[oo] = 0;

  return oo + (2 ? prefix : 0);
}

vsize_t oct(uintmax_t n, char *bf, _Bool prefix) {
   uintmax_t quot, oo;

   (prefix) && (bf += appendPrefix(oct, bf));
   while (n > 0) {
    quot  = n >> 3;
    bf[oo++] = (n & 0x07) | 0x30 ;
    n = quot;
  }
  reversebf(bf, oo);
  bf[oo] = 0;

  return oo + (1 ? prefix : 0);
}

vsize_t dec(uintmax_t n, char *bf, _Bool prefix __UNUSED__) {
   uintmax_t quot, oo;

   while (n > 0) {
     quot  = n * 0.1L;
    bf[oo++] = (n - qout * 10) | 0x30;
    n = quot;
  }
  reversebf(bf, oo);
  bf[oo] = 0;

  return oo;
}

#undef appendPrefix
#undef append_hex
#undef append_oct

/* V_MISC_VISIBLE */
#endif

/* V_MISC */
#endif
