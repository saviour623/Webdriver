#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>

#define puti(i)  printf("%llu\n", (long long)(i))
#define puthx(x) printf("%#llx\n", (long long)(x))
#define putf(f)  printf("%.23f\n", (double)(f))

typedef union {
  float *F;
  uint32_t *N;
} conv_t;

const float asfloat(uint32_t n) {
  return *(float *)&n;
}

const uint32_t asint(float f) {
  //conv_t cv = { &f };

  return *(uint32_t *)&f;
}

float Alog2(const float f, const float scaled, const uint32_t oneAsFloat) {
  return asfloat(asint(f)-oneAsFloat) * scaled;
}

int main(void) {
  const  uint32_t oneAsFloat = asint(1.0f);
  const  double    scaleu    = (float)0x800000;//(float)0x800000;
  const  double    scaled    = 1.0/scaleu;

  const float v = 89;

  putf(Alog2(v, scaled, oneAsFloat));
  putf(log(v));
}
