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
  volatile Webdriver_TMemoryPool mempool = webdriverMemoryPool();
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
  }

  {
	// Retrieving from free

	void *memory;
	size_t loSize;

	memory = webdriverMemoryPoolGet(mempool, 20000);
	loSize = (ptrdiff_t)(mempool->__tp__ - mempool->__mp__); // size left
	printf("%lu\n", loSize );

	if (memory == (void *)WEBDR_EOMEM)
	  {
		fprintf(stderr, "webdriverMemoryPoolGet: Out of memory\n");
		exit(-1);
	  }
	webdriverMemoryPoolDeleteChunk(mempool, &memory);
	memory = webdriverMemoryPoolGet(mempool, 20000);
	if (memory == (void *)WEBDR_EOMEM)
	  {
		fprintf(stderr, "webdriverMemoryPoolGet: Out of memory\n");
		exit(-1);
	  }
  }

  close(fd);
  webdriverMemoryPoolDelete(mempool);
  return 0;
}
