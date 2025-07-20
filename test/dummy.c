#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#define PUTI(n)  printf("%u\n", (int)(n))

enum {
      PTR  = 0x01,  USIGNED = 0x02,  CHAR  = 0x04,
      LONG = 0x100, LLNG    = 0x200, SIZE = 0x800,
      TYPE = 0x7f,  SPEC    = 0xf00, BASE = 0x400
};

int main(void) {
  unsigned char c, fmt[32] = "llxhu:^0.6", fc[32] = {0};
  uint32_t mask;

  mask = fc[0] == 0x6c;
  mask = (mask << (c = fc[mask] == 0x6c)) | c;
  mask = (mask << (c = fc[mask] == 0x7a)) | c;
  mask = (mask << (c = fc[mask] == 0x78)) | c;
  mask = (mask << (c = fc[mask] == 0x68)) | c;
  mask = (mask << 8) | fc[mask];

  mask = (((mask & TYPE) ^ 0x73) ^ (mask & SPEC))
    | (((mask & TYPE) ^ 0x66) ^ (mask & BASE))
    | ( (mask & SPEC)         ^ (mask & TYPE))
    |  EOFMT(c, *fmt)
    PUTI(mask);
  /*‘’
ll
    1, 3, 7, 15, 31
 2, 4, 8, 16, 32
*/
}
