#define VEC_INTERNAL_IMPLEMENTATION
#include "../v_base.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
  size_t x;
  int *intArray;
  VEC_set config = {.type = 1 , .memfill = 0};
  int **vec;
  clock_t clockTime1, clockTime2;
  char *process;
  int fd;
  char *mem;

  if(posix_memalign((void *)&mem, 32, 1325)) {
	perror("posix_memalign");
	exit(-1);
  }

  if ((fd = open("../v_base.c", O_RDONLY, S_IRUSR)) == -1) {
	process = "open";
  ioError:
	fprintf(stderr, "unable to %s file %s: %s\n", process, "../v_base.c", strerror(errno));
	return 1;
  }
  if (read(fd, mem, 1324) == -1) {
	process = "read";
	goto ioError;
  }

  clockTime2 = clock();
  memset(mem, 0, 1312);
  clockTime2 = clock() - clockTime2;

  clockTime1 = clock();
  internalMemset32Align(mem, 65, 1324);
  clockTime1 = clock() - clockTime1;

  printf("internalMemset32Align: %f\n", clockTime1/(float)CLOCKS_PER_SEC);
  printf("memset: %f\n", clockTime2/(float)CLOCKS_PER_SEC);

  putchar(mem[132]);

  puts("");

  free(mem);
  return 0;
}
