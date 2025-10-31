#include "../client.c"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int main(void)
{
  Webdriver_TMemoryPool mempool = webdriverMemoryPool();
  int fd;

  void *chunk = webdriverMemoryPoolGet(mempool, webdriverMemoryPoolMaxAlloc);

  if (NOT (chunk))
	{
	  fprintf(stderr, "MEMORY ERROR\n");
	  exit(WEBDR_EOMEM);
	}

  if ((fd = open("hashtext.txt", O_RDONLY, 777)) < 0)
	{
	  perror("OPEN");
	  exit(WEBDR_EOMEM);
	}

  if (read(fd, chunk, webdriverMemoryPoolMaxAlloc - 1) < 0)
	{
	  perror("READ");
	  exit(WEBDR_EOMEM);
	}
  {
	// TEST HASH DISTRIBUTION
	char *str = chunk, *tok;
	int hash, hashes[7] = {0}, sum = 0;
	void *ptr;
	while ((tok = strtok(str, " ")) != NULL)
	  {
		hash = webdriverObjectKeyHash(tok, 8);
		if (tok && hash != -1)
		  hashes[hash] += 1;
		str = NULL;
	}

	puts("HASH DISTRIBUTION");
	for (int i = 0; i < 7; i++)
	  sum += hashes[i];

	for (int i = 0; i < 7; i++)
	  {
		printf("+[hash %d] %d %f%%\n", i, hashes[i], (hashes[i]/(double)sum) * 100);
	  }

	/*
	  RESULTS:

	  HASH DISTRIBUTION
	  +[hash 0] 531 12.854031%
	  +[hash 1] 547 13.241346%
	  +[hash 2] 550 13.313968%
	  +[hash 3] 590 14.282256%
	  +[hash 4] 647 15.662067%
	  +[hash 5] 735 17.792302%
	  +[hash 6] 531 12.854031%

	*/
  }

  webdriverMemoryPoolDelete(&mempool);
  return 0;
}
