#include "../client.c"
#include <stdio.h>

int main(void)
{
  void *mempool = webdriverMemoryPool();
  int fd;

  void mem = webdriverMemoryPoolGet(mempool, webdriverMemoryPoolMaxAlloc);

  if (NOT (mem))
	{
	  fprintf(stderr, "MEMORY ERROR\n");
	  exit(wEBDR_EOMEM);
	}

  if ((fd = open("hashtext.txt", O_RDONLY, 777)))
	{
	  perror("OPEN");
	  exit(wEBDR_EOMEM);
	}

  if (read(fd, mem, webdriverMemoryPoolMaxAlloc) < 0)
	{
	  perror("READ");
	  exit(wEBDR_EOMEM);
	}

  for (char *str = mem, *tok; tok !=  NULL;)
	{
	  tok = strtok(str, " ");
	  tok && printf(webdriverObjecKeyHash(tok));
	  str = NULL;
	}
  printf("%d, ", webdriverObjectKeyHash(, 8));

  printf("%u\n", index);
  return 0;
}
