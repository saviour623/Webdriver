#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <immintrin.h>

// _mm_empty()
uint16_t find(uint8_t *m16)
{
}

int main(void)
{
  _Alignas(16) uint8_t ones[16] = {9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9};

  __m128i ee = _mm_load_si128((void *)ones);
  ee = _mm_add_epi8(ee, ee);
  //*(uint64_t *)ones = _mm_cvtm64_si64(ee);

#if 0
  for (int i = 0; i < 8; i++)
	fprintf(stderr, "%d\n", ones[i]);
#endif

  __m64 pp = _mm_cvtsi64_m64(12312324ULL);
  pp = _mm_packs_pi32(pp, pp);
  *(uint64_t *)ones = _mm_cvtm64_si64(pp);

  // load
  __m128i oo = _mm_stream_load_si128((void *)ones); //_mm_lddqu_si128 - unaligned
  oo = _mm_and_si128(oo, oo);
  _mm_extract_epi8(oo,1);

  //_mm_and_si128(a, b)
  //_mm_andnot_si128(a, b) -> ~a & b
  //_mm_or_si128(a, b)
  //_mm_xor_si128(a,b)

  //_mm_slli_si128(a, shift) shift left (signed)
  //_mm_slli_epi128(a, shift) shift left (unsigned)
  //_mm_srli_si128(a, shift) shift right (signed)
  //_mm_srli_epi128(a, shift) shift right (unsigned)

  //_mm_cmpeq/gt/lt_epi8/16/32(a, b)

  //_mm_cvtsi32/64/128_si128/64/32(int32/int64/_m128i a) move a into the lower bitprec(a), zero the remaining upper bits

  //__m128i _mm_load_si128(_m128i *const p) (16byte aligned)
  //_mm_loadu_si128 (unaligned)
  //_mm_loadl_epi64 (load lower 64 bit, zero the upper bits)

  // SET
  //setzero_si128()
  
  //__m128i _mm_set_epi64(__m64 a, __m64 b)
  //set1_epi64(__m64 a) duplicate a into two

  //set_epi64x(int64 a, int64 b)
  //set1_epi64x(int64 a)

  //set_epi32(int32 a, b, c, d)
  //set1_epi32(int32 a)

  //set_epi16(int16, a, b, c, d, e, f, g)
  //set1_epi16

  //set_epi8(int8...)
  //set1_epi8

  //setr_epi64/32/16/8 (set in reverse order)

  //STORE
  //void _mm_stream_si128(__m128i *p/void *p, __m128i a) store a in address p without polluting caches (p is 16byte aligned)
  //  store_si128(void *p, __m128i p) //aligned
  // storeu_si128 //unaligned
  // _mm_maskmoveu_si128(__m128i d, __m128i mask, void *p) -> store only byte elements of d to address p.the high bit of each byte of mask will determine if the corresponding byte will be stored.
  //_mm_store1_epi64(*p, a) store lower 64 bit of a in p


  //int32 _mm_extract_epi64/32/16/8(__m128i a, int index)
  //__m128i _mm_insert_epi64/32/16/8(__m128i a, int index)

  //int movemask_epi8/32/64(__m128i a) //create a mask from the MSB bit of the sixteen N bit integers

  //__m128i _mm_shuffle_epi32(__m128i a, int32 mask)
  //__m128i shufflehi_epi16
  //__m128i shufflelo_epi16

  //movepi64_pi64

  uint8_t *meta = (uint8_t[16]){4,6,7,5,4,4,5,4,4,4, 6, 0, 0,4,4, 5};
  uint8_t re[16];
  const uint8_t id = 4;
  __m128i mask;
  __m128i _main;

  _main = _mm_load_si128((void *)meta);
  mask  = _mm_cmpeq_epi8(_main,
						_mm_set1_epi8(id)
						);
  mask = _mm_subs_epu8(mask, _mm_set_epi8(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15));
  _mm_maskmoveu_si128(_main, mask, re);

  printf("%u\n", re[4]);
}
