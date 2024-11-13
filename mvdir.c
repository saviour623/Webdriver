#include <fcntl.h>
#include "mvdef.h"

#define _GNU_SOURCE
#define S_DOTREF_2DOTREF(s)												\
	(((s) != NULL) && ((*(s) != '\0')) && (strlen(s) <= 2) ? (*(s) == '.' && *((s) + 1) == '.') || (*(s) == '.') : 0)

#define bool char
#define TRUE ((char)1)
#define FALSE ((char)0)
#define MVPROG_END_OF_MACRO_FUNC(...) (void)0

#define MVPROG_RECALL_FUNC(func, ...) func(__VA_ARGS__)
#define MVPROG_PERROR_RET(cond, str, ...) do { if ((cond)) { perror(str); return __VA_ARGS__; }} while (0)
#define MVPROG_MAX_MEM_ALLOC (INT_MAX - 1)

#define mvprog_do_nothing() (void)0

extern char **environ;

void *mvprog_malloc(void *ptr, size_t n, int reAlloc) {
	void *mp = NULL;

	if (n < 1) {
		return NULL;
	}
	if (ptr == NULL) {
		mp = malloc(n);
	}
	else if (reAlloc) {
		mp = realloc(ptr, n);
	}
	/* couldn't allocate memory */
	if (mp == NULL) {
		fprintf(stderr, "malloc: out of memory");
		return NULL;
	}
	ptr = mp;
	return mp;
}
#define _MVPROG_EMPT_BUF
typedef int8_t MVPROG_8BINT;
MVPROG_STR mvprog_getcwd (MVPROG_STR *buf, ssize_t *bufsz) {
	MVPROG_STR gtcwd = NULL;
	MVPROG_STR cwd;
	MVPROG_STR *env_ptr;
	MVPROG_8BINT is_null = (buf == NULL) || (*buf == NULL);
	register ssize_t len = 0;

	if (!bufsz || ((*bufsz < 0) && is_null) || (*bufsz >= MVPROG_MAX_MEM_ALLOC)) {
		/* bufsz must be set correctly to the size allocated to buf */
		return (MVPROG_STR)0;
	}

	env_ptr = environ;
	
	while ((gtcwd = *env_ptr++)) {
#if !__assert_pwd
#define __assert_pwd(c) (((c)[0] == 'P') && ((c)[1] == 'W') && ((c)[2] == 'D') && ((c)[3] == '='))
		if (__assert_pwd(gtcwd)) {
			gtcwd += 4;
			break;
		}
#undef assert_pwd
#endif
	}
	if ((gtcwd == NULL) || (*gtcwd == '\0')) {
		if (is_null) {
			cwd = (MVPROG_STR)mvprog_malloc(NULL, PATH_MAX, 0);
			if (cwd == NULL)
				return (MVPROG_STR)0;
			*bufsz = PATH_MAX;
		}
		else
			cwd = *buf;
	re:
		errno = 0;
		if ((getcwd(cwd, PATH_MAX) == NULL)) {
			if (errno == ERANGE) {
				if ((MVPROG_STR)mvprog_malloc(cwd, *bufsz + 256, 1) == NULL)
					return (MVPROG_STR)0;
				*bufsz += 256;
				goto re;
			}
			MVPROG_PERROR_RET(TRUE, "mvprog_getcwd", (MVPROG_STR)0);
		}

		/* "(unreachable)" attached by linux(>2.6.36) when accessing a dir below root of the process */
		if (*cwd == '(') {
			fprintf(stderr,"mvprog_getcwd: unreachable");
			return (MVPROG_STR)0;
		}
	}
	else {
		len = strlen(gtcwd) + 1;

		if (is_null) {
			cwd = (MVPROG_STR)mvprog_malloc(buf, len, 0);

			if (cwd == NULL)
				return (MVPROG_STR)0;
			*bufsz = len;
		}
		else if (len > *bufsz) {
			cwd = mvprog_malloc(*buf, len, 1);
			if (cwd == NULL)
				return (MVPROG_STR)0;
			*bufsz = len;
		}
		if (strcpy(cwd, gtcwd) == NULL) {
			fprintf(stderr, "mvprog_getcwd: strcpy: unable to copy");
			return (MVPROG_STR)0;
		}
		cwd[len] = 0;
	}
	return is_null ? cwd : (*buf = cwd), *buf;
}

MVPROG_INTTYPE main(int argc __attribute__((unused)), char **argv __attribute__((unused))) {
	int fd;
	if (mkdir("newdir", S_IRWXU | S_IRGRP) && (errno != EEXIST)) {
		perror("");
		exit(-1);
	}
	if ((symlink("newdir", "symlink_newdir")) && (errno != EEXIST)) {
		perror("");
		exit(-1);
	}
	if ((fd = open("/", O_DIRECTORY, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH)) && (errno != EEXIST)){
		puts("here");
		perror("open");
		return -1;
	}
	char *p = mvprog_getcwd(NULL, NULL);
	puts(p ? p : "null");
	//char *pd = malloc(8);
	//pd = realloc(pd, 2);
	return MVPROG_SUCC;
}
