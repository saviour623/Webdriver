#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../include.h"

#define PUTI(n)  printf("%u\n", (int)(n))


#define IS_INT(mask) (mask & 0b111100000u)
#define IS_FLOAT(mask) (mask &   0b11000000000u)
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

#define JMP(M);
#define LOCATION(M) 0
int main(void) {
  uint32_t mask;

  mask = formatMask("d");

  if (IS_INT(mask)) {
	LOCATION(IS_INT);
  }

  if (IS_FLOAT(mask))
    ;

  if (IS_CHARS(mask))
    ;

  if (IS_ADDRS(mask)) {
	mask |= 0;
	JMP(JMP_ADDRS);
  }

  PUTI(mask);

  return 0;
}
