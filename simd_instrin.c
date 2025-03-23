#ifdef __ARM_NEON
#include <arm_neon>
//#elif defined(__i386__) || defined(__i386) || defined(x86_64)
#else
#include <mmintrin.h>
#include <immintrin.h>
#include <x86intrin.h>
#include <xmmintrin.h>
#endif
#include <stdint.h>
#include <stdio.h>

__m256i __add(uint16_t __a[], uint16_t __b) {
  
}
int main(void) {
  float ft[4] = {1.1, 1.2, 1.3, 1.4};
  __m128 ftl = _mm_load_ps(ft);
  //int *cast = (int *)&_ibbfr;

  //printf("%d %d %d %d %d %d %d %d\n", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

}
