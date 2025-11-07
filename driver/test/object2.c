#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <immintrin.h>

#define USE_SIMD
#define NOT(e) (!(e))
#define puti(i) fprintf(stderr, "%llu\n", (long long)(i))

struct Tobject {
  uint8_t __obmeta__[16];
  uint8_t *__obdata__[13];
  uint8_t *__obnext__;
};

static __inline__ __attribute__((always_inline, pure)) int objectGetId(const char *key)
{
  uint32_t mask = 0, e = __builtin_strlen(key);

  if (e < 4)
	{
	  mask = 5381;
	  switch (e)
		{
		case 3: mask = (mask << 5 + mask) + *key++;
		case 2: mask = (mask << 5 + mask) + *key++;
}
	  return ((mask << 5 + mask) + *key) % 13;
	}
  mask = *(uint16_t *)key;
  mask |= ((uint32_t)*(uint16_t *)(key + e - 3) << 16);
  mask = ((mask * 0x5a2b0f0f) + (uint32_t)((key[(uint32_t)(e * 0.56f)]) << 3));

  return (mask + (0xe2e3e11 * key[e-1])) % 13;
}

static const __inline__  __attribute__((always_inline)) uint8_t ObjectFindKey(const struct Tobject *object, const uint8_t *key, const uint8_t id)
{
  uint8_t *meta__ = object->__obmeta__, **obdata__ = object->__obdata__, *obkey __attribute__((unused)) = NULL;
  //*(uint64_t *)meta__ &= 0xffffffffffULL;
#ifdef USE_SIMD
  uint16_t mask = _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_load_si128((void *)meta__), _mm_set1_epi8(id))), idx = 0;
  do
	{
	  idx   = __builtin_ctz(mask);
	  obkey = obdata__[idx];
	  mask  &= (0b1111111111110u << idx);
	} while ((NOT(*obkey ^ *key) || strcmp(obkey, key)) && mask);
  return idx;
#else
  //
#endif
  return 0;
}

int main(void)
{
  struct Tobject *object = malloc(sizeof(struct Tobject));

  if (object == NULL)
	{
	  perror("malloc");
	  exit(-1);
	}

  char *keys[13] = {
				    "063dyjuy",
					"070462",
					"085tzzqi",
					"10th",
					"11235813",
					"12qwaszx",
					"13576479",
					"135790",
					"142536",
					"142857",
					"147258",
					"14725836",
					"9678787"
  };

  for (int i = 0; i < 13; i++)
	{
	  (object->__obdata__)[i] = keys[i];
	}

   _mm_storeu_si128((void *)(object->__obmeta__), _mm_set1_epi8(4));

   printf("%u\n", ObjectFindKey(object, "147258", 4));

   return 0;
}
