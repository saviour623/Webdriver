#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#define PUTI(n)  printf("%u\n", (int)(n))

enum {
      PTR  = 0x01,  USIGNED = 0x02,  CHAR  = 0x04,
      LONG = 0x100, LLNG    = 0x200, SIZE = 0x800,
      TYPE = 0x7f,  SPEC    = 0xf00, BASE = 0x400
};
#define EOFMT(c, f) (((c) = (f)) & ((c) ^ 58))
int main(void) {
  unsigned char c, b, fmt[32] = "llxhu:^0.6", fc[32] = {0};
  uint32_t mask, error;

  b = 3;
  fc[0] = 'l';
  fc[1] = 'l';
  fc[3] = 'u';
  //fc[3] = 'u';

  /* mask = fc[0] == 0x6c; */
  /* mask = (mask << (c = fc[mask] == 0x6c)) | c; */
  /* mask = (mask << (c = fc[mask] == 0x7a)) | c; */
  /* mask = (mask << (c = fc[mask] == 0x78)) | c; */
  /* mask = (mask << (c = fc[mask] == 0x68)) | c; */
  /* mask = (mask << 8) | fc[mask]; */

#define aeqshl(l, r, s, c) ((c=!((l) ^ (r))) >> s)

  mask  = (c=fc[0] == 0x68);
  // 0..1...2...4...8...16
  mask |= (c=(fc[c] == 0x6c) << (1 >> c));
  mask |= c & (c=(fc[c] == 0x6c) << 2);
  mask  = !c & (c=(fc[c] == 0x7a) << 3);
  mask |= (c=(fc[c] == 0x78) <<)

  /* Check if No format specifier is ignored (invalid at position) and there are no left over characters after format chars */
  /* #define IgnoredMaskOrFormat(M, L, F, C)\ */
  /*   (!((M) >> ((L)+6)) || EOFMT(C, F)) */

  /*   error = ( */
  /* 	      (((mask & TYPE) == 0x73) && (mask & SPEC)) */
  /* 	    | (((mask & TYPE) == 0x66) && (mask & BASE)) */
  /* 	    | (!(mask & TYPE)          && (mask & SPEC)) */
  /* 	    | IgnoredMaskOrFormat(mask, b, *fmt, c) */
  /* 	    ); */

  /* #undef IgnoredMaskOrFormat */

 label: 0;
      /*‘’
ll || EOFMT(C, F)
    1, 3, 7, 15, 31
 2, 4, 8, 16, 32
*/
}
