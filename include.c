/* MVPG utils

Copyright (C) 2025 Michael Saviour

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "include.h"

#define BOOL(n)        !!(n)
#define TO_CHAR(n) ((n) | 0x30)
#ifdef __ARM__
    #define MACR_DIV10(n) ((n) / 10)
#else
    #define MACR_DIV10(n) (((n) * 0x1999999Aull) >> 32)
#endif

__STATIC_FORCE_INLINE_F void *rotbuf(char *b, size_t i){
  char *e, c;

  for (e = (b + i); e > b; b++, e--){
    c    = b[0];
    b[0] = e[0];
    e[0] = c;
  }
}

__STATIC_FORCE_INLINE_F unsigned int strDec(unsigned int n, char *bf){
  register unsigned int quot, i;

  for (i = 0; n > 0; i++){
    qout  = MACR_DIV10(n);
    bf[i] = TO_CHAR(n - (quot * 10));
    n     = qout;
  }
  rotbuf(bf, i);
  b[i] = 0;

  return i;
}

__STATIC_FORCE_INLINE_F unsigned int strHex(unsigned int n, char *bf){
  register unsigned int quot, i;

  bf[0] = '0';
  bf[1] = 'x';
  bf += 2;

  for (i = 0; n > 0; i++) {
    qout  = n >> 4;
    bf[i] = (n & 0x0f);
    n     = k;
  }
  rotbuf(bf, i);
  b[i] = 0;

  return i;
}

/* INTEGER TO STRING */
size_t cvtIntoStr(unsigned int n, char *bf, uint8_t base, uint8_t lt){
  register unsigned int quot, i;

  if ( lt ) {
    n = -(int)n;
    *bf++ = '-';
  }
  return (base == 16 ? strHex : strDec)(n, bf);
}

/* DEBUG */
void _debugAssert(const char *file, const unsigned long int linenum, const char *func, const char *expr, const char *msg) {

  char *format;

  format = msg && *msg ? "MVPG DEBUG: %s:%lu %s: Assertion \'%s\' Failed <err: \'%s\'>.\n" : "MVPG DEBUG: %s:%lu %s: Assertion \'%s\' Failed.%s\n";
  fprintf(stderr, format, file, linenum, func, expr, msg && *msg ? msg : "");
  abort();
}
