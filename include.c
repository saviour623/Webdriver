/* MVPG utils

Copyright (C) 2025 Michael Saviour

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "include.h"

#define BOOL(n)        !!(n)
#define TO_CHAR(n) ((n) | 0x30)

#ifndef __ARM_CPU__
    #define MACR_DIV10(n) ((n) / 10)
#else
    #define MACR_DIV10(n) (((n) * 0x1999999Aull) >> 32)
#endif

__STATIC_FORCE_INLINE_F void *rotbuf(char *b, size_t i){
  char *e, c;

  for (e = b + i - 1; b < e; b++, e--){
    c    = b[0];
    b[0] = e[0];
    e[0] = c;
  }
}

__STATIC_FORCE_INLINE_F uintmax_t strDec(uintmax_t n, char *bf){
  register uintmax_t quot, i;

  for (i = 0; n > 0; i++){
    quot  = MACR_DIV10(n);
    bf[i] = TO_CHAR(n - (quot * 10));
    n     = quot;
  }
  rotbuf(bf, i);
  bf[i] = 0;

  return i;
}

__STATIC_FORCE_INLINE_F uintmax_t strHex(uintmax_t n, char *bf){
  register uintmax_t quot, i;

  bf[0] = '0';
  bf[1] = 'x';
  bf += 2;

  for (i = 0; n > 0; i++) {
    quot  = n >> 4;
    bf[i] = "0123456789abcdef"[(n & 0x0f)];
    n     = quot;
  }
  rotbuf(bf, i);
  bf[i] = 0;

  return i;
}

/* INTEGER TO STRING */
uintmax_t cvtInt2Str(uintmax_t n, char *bf, uint8_t base, uint8_t lt){

  if ( lt ) {
    n = -(intmax_t)n;
    *bf++ = '-';
  }
  return (base == 16 ? strHex : strDec)(n, bf) + !!lt;
}

/* DEBUG */
void _debugAssert(const char *file, const unsigned long int linenum, const char *func, const char *expr, const char *msg) {

  char *format;

  format = msg && *msg ? "MVPG DEBUG: %s:%lu %s: Assertion \'%s\' Failed <err: \'%s\'>.\n" : "MVPG DEBUG: %s:%lu %s: Assertion \'%s\' Failed.%s\n";
  fprintf(stderr, format, file, linenum, func, expr, msg && *msg ? msg : "");
  abort();
}
