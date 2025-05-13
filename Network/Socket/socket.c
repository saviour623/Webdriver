#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


int main(void) {
  int status;
  struct addrinfo netinfoHint;
  struct addrinfo *servinfo;

  memset(&netinfoHint, 0, sizeof netinfoHint);
  netinfoHint.ai_family = AF_INET;
  netinfoHint.ai_socktype = SOCK_STREAM;
  netinfoHint.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo))) {
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
	exit(EXIT_FAILURE);
  }

  freeaddrinfo(servinfo);
  return 0;
}
