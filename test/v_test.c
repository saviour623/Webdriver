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
  void *mem;

  posix_memalign(&mem, 32, 1024);

  if ((fd = open("../v_base.c", O_RDONLY, S_IRUSR)) == -1) {
	process = "open";
  ioError:
	fprintf(stderr, "unable to %s file %s: %s\n", process, "../v_base.c", strerror(errno));
	return 1;
  }
  if (read(fd, mem, 1024) == -1) {
	process = "read";
	goto ioError;
  }

  clockTime1 = clock();
  internalMemset32Align(mem, 0, 1024);
  clockTime1 = clock() - clockTime1;

  clockTime2 = clock();
  memset(mem, 0, 1024);
  clockTime2 = clock() - clockTime2;

  printf("internalMemset32Align: %f\n", clockTime1/(float)CLOCKS_PER_SEC);
  printf("memset: %f\n", clockTime2/(float)CLOCKS_PER_SEC);
  // vec = (void *)VEC_new(131072, config);
  //VEC_delete((void *)&vec);
  return 0;
}
