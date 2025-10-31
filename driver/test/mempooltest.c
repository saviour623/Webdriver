#include <stdio.h>
#include "../client.c"
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <stddef.h>
#define STARTTIME(t) ((t) = clock())
#define STOPTIME(t)  ((t) = clock() - (t))
#define PRINTTIME(t) printf("%f\n", t/(double)CLOCKS_PER_SEC)

#define MEMPRINT(mem) fprintf(stderr, "(MEMPRINT)\n`@%s <%p>`\n", #mem, (void *)(mem))

size_t freeCount(void **freelist)
{
  void **loop = freelist;
  size_t counter = 0;

  while (loop)
	{
	  counter++;
	  loop = *loop;
	}

  return counter;
}

int main(void) {
  Webdriver_TMemoryPool mempool;
  int fd;
  clock_t timer;

  fd = open("text.txt", O_RDONLY, 777);
  if  (fd < 0)
	{
	  perror("open");
	  exit(WEBDR_FAILURE);
	}

  {
	//GET AND DELETE
#if 0
	mempool = webdriverMemoryPool();
	void *memory = webdriverMemoryPoolGet(mempool, 20000);
	size_t cCHUNK = 12080 >> 4, FcCHUNK = 0;

	if (memory == (void *)WEBDR_EOMEM)
	  {
		fprintf(stderr, "webdriverMemoryPoolGet: Out of memory\n");
		exit(-1);
	  }

	if (read(fd, memory, 19999) < 0)
	  {
	    perror("read");
		exit(-1);
	  }

	//puts(memory);
	MEMPRINT(mempool);
	webdriverMemoryPoolDeleteChunk(mempool, &memory);

	STARTTIME(timer);
	for (uint16_t i = 0; i < cCHUNK; i++)
	  {
		memory = webdriverMemoryPoolGet(mempool, 16);
		webdriverMemoryPoolDeleteChunk(mempool, &memory);
	  }
	STOPTIME(timer);
	PRINTTIME(timer);

	STARTTIME(timer);
	for (uint16_t i = 0; i < cCHUNK; i++)
	  {
		memory = malloc(16);
	    free(memory);
	  }
	STOPTIME(timer);
	PRINTTIME(timer);

	FcCHUNK = freeCount(mempool->__free__) - 1;

	if (cCHUNK != FcCHUNK)
	  {
		fprintf(stderr, "<error> number of allocated chunks and equivalent frees differ \nCHUNK: %lu\nFCHUNK: %lu\n", cCHUNK, FcCHUNK);
		exit(-1);
		}
	webdriverMemoryPoolDelete(&mempool);
#endif
  }

  {
	// Retrieving from free
#if 0
	void *memory;
	size_t loSize;

	mempool = webdriverMemoryPool();
	memory = webdriverMemoryPoolGet(mempool, webdriverMemoryPoolMaxAlloc);
	loSize = (ptrdiff_t)(mempool->__tp__ - mempool->__mp__); // size left

	printf("%lu\n", loSize );

	if (memory == (void *)WEBDR_EOMEM)
	  {
		fprintf(stderr, "webdriverMemoryPoolGet: call1: Out of memory\n");
		exit(-1);
	  }
	webdriverMemoryPoolDeleteChunk(mempool, &memory);

	if (mempool->__free__ == NULL)
	  {
		fprintf(stderr, "webdriverMemoryPoolGet: Free Failed\n");
		exit(-1);
	  }
	memory = webdriverMemoryPoolGet(mempool, webdriverMemoryPoolMaxAlloc);
	if (memory == (void *)WEBDR_EOMEM)
	  {
		fprintf(stderr, "webdriverMemoryPoolGet: call2: Out of memory\n");
		exit(-1);
	  }

	// freelist must be empty
	if (mempool->__free__ != NULL)
	  {
		fprintf(stderr, "webdriverMemoryPoolGet: Something Went wrong\n");
		exit(-1);
	  }

	lseek(fd, SEEK_SET, 0);
	if (read(fd, memory, 19999) < 0)
	  {
	    perror("lseek/read");
		exit(-1);
	  }

	//puts(memory);
	webdriverMemoryPoolDelete(&mempool);
#endif
  }

  {
	// Grow Chunk
	void *memory;
	size_t loSize;

	mempool = webdriverMemoryPool();
	memory = webdriverMemoryPoolGet(mempool, 4);

	memcpy(memory, "four", 4);

	if (webdriverMemoryPoolGrowChunk(mempool, &memory, 8) == (void *)WEBDR_EOMEM)
	  {
		fprintf(stderr, "webdriverMemoryPoolGrowChunk: Memory Error\n");
		exit(-1);
	  }
	memcpy(memory, "fourByte", 8);
	//puts(memory);

	if (chunkSize(memory) != 8)
	  {
		fprintf(stderr, "webdriverMemoryPoolGrowChunk: Invalid resize\n");
		exit(-1);
	  }

	if (webdriverMemoryPoolGrowChunk(mempool, &memory, 256) == (void *)WEBDR_EOMEM)
	  {
		fprintf(stderr, "webdriverMemoryPoolGrowChunk: Memory Error\n");
		exit(-1);
	  }

	if (chunkSize(memory) == 8)
	  {
		fprintf(stderr, "webdriverMemoryPoolGrowChunk: Invalid resize\n");
		exit(-1);
	  }
	printf("chunk-size: %d\n", chunkSize(memory));
	webdriverMemoryPoolDelete(&mempool);
  }

  close(fd);
  return 0;
}
