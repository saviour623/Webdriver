#include "json.h"
#include <stdio.h>

int main(int argc, char **argv) {
	int fd;
	char bf[1025];
	ssize_t nr;
	if (argc != 1) {
		return -1;
	}

	if ((fd = open(argv[argc], O_RDONLY))) {
		fprintf(stderr, "%s\n", strerror(errno));
		return -1;
	}
	if ((nr = read(fd, bf, 1024)) == -1) {
		fprintf(stderr, "%s\n", strerror(errno));
		return -1;
	}
	bf[nr] = 0;

	if (lexer((JsonObject)1, "{\"name\": \"Gracias\"}", 17) == NULL) {
		puts("error");
	}
	return 0;
}
