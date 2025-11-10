//#include "../client.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#define NOT(e) !(e)
uint8_t getId2(char *key)
{
  unsigned long hash = 5381;
  int c;
  int len = strlen(key);

  while ((c = *key++))
	{
	  hash = ((hash << 5) + hash) + c;
	}
  return hash % 14;
}

static __inline__ __attribute__((always_inline, pure)) int 
getId(char *key)
{
  uint32_t mask = 0, e = __builtin_strlen(key);

  if (e < 2)
	return (177573ull + *key) % 14;

  mask = *(uint16_t *)key;
  mask += ((uint32_t)*(uint16_t *)(key + (e >> 1) - 1))<<8;
  mask = (mask * 17991u) + key[14 & (e - 1)];
  //17679
  return (mask + (0x7266218736ull * key[e-1])) % 14;
//0x5a2b0f0f
//0xe2e3e1
}
int main(void)
{
 // Webdriver_TMemoryPool mempool = webdriverMemoryPool();
  int fd;

  //void *chunk = webdriverMemoryPoolGet(mempool, 
int webdriverMemoryPoolMaxAlloc = 65500;
void *chunk = malloc(webdriverMemoryPoolMaxAlloc);
  if (NOT (chunk))
	{
	  fprintf(stderr, "MEMORY ERROR\n");
	  exit(-1);
	}

  if ((fd = open("words", O_RDONLY, 777)) < 0)
	{
	  perror("OPEN");
	  exit(-1);
	}
#if 0
  if (read(fd, chunk, webdriverMemoryPoolMaxAlloc - 1) < 0)
	{
	  perror("READ");
	  exit(-1);
	}
#endif
  {
	int hash, hashes[16] = {0}, sum = 0;
	void *ptr;
	char *str = chunk, *tok;

	//printf("%llu\n", ~(uint64_t)0x5a2b0f0f);
	while (read(fd, chunk, webdriverMemoryPoolMaxAlloc - 1) > 0)
	  {
		// TEST HASH DISTRIBUTION
		str = chunk;

		while ((tok = strtok(str, "\n")) != NULL)
		  {
		   hash = getId(tok);
			//hash = djb2hash13(tok);
			if (tok && hash != -1)
			  hashes[hash] += 1;
			//fprintf(stderr, "%d, ", hash);
			str = NULL;
		  }
	  }

	puts("||| HASH DISTRIBUTION |||\n");

	for (int i = 0; i < 14; i++)
	  sum += hashes[i];

	for (int i = 0; i < 14; i++)
	  {
		printf("+[hash %d] %d %f%%\n", i, hashes[i], (hashes[i]/(double)sum) * 100);
	  }

	/*
	  RESULTS: len(word) => 4

	  +[hash 0] 4308 8.008179%
	  +[hash 1] 3975 7.389163%
	  +[hash 2] 4142 7.699600%
	  +[hash 3] 4211 7.827865%
	  +[hash 4] 4144 7.703318%
	  +[hash 5] 4105 7.630821%
	  +[hash 6] 3979 7.396598%
	  +[hash 7] 4239 7.879914%
	  +[hash 8] 4079 7.582489%
	  +[hash 9] 4188 7.785110%
	  +[hash 10] 4099 7.619667%
	  +[hash 11] 4236 7.874338%
	  +[hash 12] 4090 7.602937%
	*/
  }

  // TODO: (i) there is a leak (1 byte write) (ii) test hash function against other hash functions
  return 0;
}
