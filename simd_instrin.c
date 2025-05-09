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

typedef __m128i m128i_t;
__m256i __add(uint16_t __a[], uint16_t __b) {
  
}
int main(void) {
 

}

/*
 * load the string
 * compare for zero
 * we need to know the loation of the zero
 */
