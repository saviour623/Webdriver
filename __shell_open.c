#if __GNUC__
#define _GNU_SOURCE
#define __UNUSED__  __attribute__((unused))
#endif
#if ! defined(__UNUSED__)
#define __UNUSED__
#endif
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

static __inline__ int is_uint(char *s) {
    register unsigned char c;
    register int fnd_num = 0;

    if (s == NULL || *s == 0) {
	return -1;
    }

#define CHECK_FOR_SPC_R_NUM(c)\
    ((c) == 43 || (c) == 45 || (c) == 48)

    for (; (c = *s) && CHECK_FOR_SPC_R_NUM(c); s++) {
	if (((c) > 47 & (c) < 58))
	    fnd_num = 1;
	if (c == 45) {
	    /* negative number */
	    fnd_num = 0;
	    break;
	}
    }
    return fnd_num;
}
static __inline__ unsigned int postcheck_to_uint(char *s) {
    register unsigned char c;
    unsigned int num = 0;
    
    if (s == NULL || *s == 0) {
	return -1;
    }
    while ((c = *s++)) {
	num = (num * 10) + ((unsigned int)c - 48);
    }
    return num;
}

int main(int argc, char *argv[]) {
    int fd __UNUSED__;
    pid_t pid;
    if (argc < 3) {
	printf("%d\n", -1);
	return 1;
    }
    /* create orpan to keep file descriptor valid */
    fflush(stdout);

    switch ((pid = fork())) {
    case -1:
	printf("%d", -1);
	exit(-1);
    case 0:
	fflush(stdout);
	char bf[1024];
	/* if scheduled first allow parent to exit */
	//while (getppid() != 1)
	//  ;

	fd = -1;
	if ( !strcmp(argv[1], "--open")) {
	    fd = open(argv[argc - 1], O_RDONLY);

	    if (fd == -1)
		goto __inner_error;

	    fprintf(stdout, "[%d %ld]", fd, (long)getpid());
	}
	else if (!strcmp(argv[1], "--close") && is_uint(argv[1])) {
		goto __inner_error;

	    if (execl("pkill", argv[argc - 1], NULL) == -1)
		goto __inner_error;
	    /* exec won't get here if successful */
	    
	__inner_error:
	    fprintf(stdout, "%d\n", -1);
	    _Exit(1);
	}
    default:
	fprintf(stdout, "%d\n", pid);
    }
    return 0;
}
