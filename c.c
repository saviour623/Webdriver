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

extern char **environ;

void *mvprog_malloc(void *ptr, size_t n, int reAlloc) {
	if (n < 1) {
		return NULL;
	}
	if (ptr == NULL) {
		ptr = malloc(n);
	}
	if (ptr && reAlloc) {
		ptr = realloc(ptr, n);
	}
	/* couldn't allocate memory */
	if (ptr == NULL) {
		fprintf(stderr, "malloc: out of memory");
		return NULL;
	}
	return ptr;
}
MVPROG_STR mvprog_getcwd (MVPROG_STR buf, ssize_t *bufsz) {
	MVPROG_STR gtcwd = getenv("PWD");
	MVPROG_STR cwd = buf;
	register ssize_t len __attribute__((unused)) = 0;
	int null_tracker = (buf == NULL);

	gtcwd = environ;
	for (; (gtcwd != NULL); gtcwd++) {
		if ((gtcwd[0] == 'P') && (gtcwd[1] == 'W') && (gtcwd[2] == 'D') && (gtcwd[3] == '=')) {
			*gtcwd += 4;
		}
	}
	puts(*gtcwd);
	puts(*environ);
	return NULL;
	if ((*bufsz < 0) && (buf == NULL))
		bufsz = 0;
	if (((*bufsz < 1) && (buf != NULL)) || (*bufsz >= MVPROG_MAX_MEM_ALLOC)) {
		/* bufsz must be set correctly to the size allocated to buf */
		return (MVPROG_STR)0;
	}
	if ((gtcwd == NULL) || (*(gtcwd) == '\0')) {
		if (buf == NULL) {
			cwd = (MVPROG_STR)mvprog_malloc(NULL, PATH_MAX, 0);
			if (cwd == NULL)
				return (MVPROG_STR)0;
			*bufsz = PATH_MAX;
		}
	re:
		errno = 0;
		if ((getcwd(cwd, PATH_MAX) == NULL)) {
			if (errno == ERANGE) {
				if ((MVPROG_STR)mvprog_malloc(cwd, 256, 1) == NULL)
					return (MVPROG_STR)0;
				bufsz += 256;
				goto re;
			}
			MVPROG_PERROR_RET(TRUE, "mvprog_getcwd", (MVPROG_STR)0);
		}
		/* "(unreachable)" attached by linux(>2.6.36) when accessing a dir below root of the process */
			if (*cwd == '(') {
				fprintf(stderr,"mvprog_getcwd: unreachable");
				return (MVPROG_STR)0;
			}
			return null_tracker ? cwd : ((buf = cwd), buf);
	}
	else {
		len = strlen(gtcwd) + 1;

		if (buf == NULL) {
			cwd = (MVPROG_STR)mvprog_malloc(buf, len, 0);

			if (cwd == (MVPROG_STR)0)
				return (MVPROG_STR)0;
			*bufsz = len;
		}
		else if (len > *bufsz) {
			cwd = (MVPROG_STR)mvprog_malloc(buf, len - *bufsz, 1);
			if (cwd == (MVPROG_STR)0)
				return (MVPROG_STR)0;
			*bufsz = len;
		}

		if ((MVPROG_STR)strcpy(cwd, gtcwd) == (MVPROG_STR)NULL) {
			fprintf(stderr, "mvprog_getcwd: strcpy: unable to copy");
			return (MVPROG_STR)0;
		}
		return ((cwd[len] = '\0'), (null_tracker ? cwd : (buf = cwd), buf));
	}
}
void mvprog_read_dir(MVPROG_INTTYPE fd) {
	DIR *dir = NULL;
	struct dirent *entry;
	static char *c_entry_name;
	static char *cwd = (char *)0;
	static unsigned char alloc_mem = FALSE;
	struct stat statbuf __attribute__((unused));
	static int efd __attribute__((unused));
	static int recursive_count = 0;

	MVPROG_PERROR_RET(!(dir = fdopendir(fd)), "fdopendir");

	while ((entry = readdir(dir)) != NULL) {
		c_entry_name =  entry->d_name;
		if (S_DOTREF_2DOTREF(c_entry_name)) {
			continue;
		}
#define RECURSIVELY_CALL_MVPROG_READ_DIR(...)							\
		if (!alloc_mem) {												\
			if ((cwd = (char *)malloc(1024)) == NULL) {					\
				fprintf(stderr, "malloc: could not allocate memory");	\
				exit(MVPROG_FAILURE);									\
			}															\
			alloc_mem = TRUE;											\
		}																\
		/* store current working directory in cwd */					\
		MVPROG_PERROR_RET((getcwd(cwd, 1024) == NULL), "getcwd");		\
		/* temporarily change directory to fd to directly access c_entry_name */ \
		MVPROG_PERROR_RET(fchdir(fd), "fchdir");						\
		MVPROG_PERROR_RET(((efd = open(c_entry_name, O_DIRECTORY, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH)) && (errno != EEXIST)), "open"); \
		recursive_count += 1;											\
		/* recursively call function */									\
		MVPROG_RECALL_FUNC(mvprog_read_dir, efd);						\
		MVPROG_END_OF_MACRO_FUNC(NULL)

#if defined(_BSD_SOURCE) || defined(_DIRENT_HAVE_D_TYPE)
		/* bsd systems and some gblic implementation provides a d_type assigned with the type of the file */
		if ((entry->d_type == DT_DIR)) {
			RECURSIVELY_CALL_MVPROG_READ_DIR(NULL);
		}
#else
		/* use stat to get file type */
		MVPROG_PERROR_RET(lstat(c_entry_name, &statbuf), "lstat");

		if (S_ISDIR(statbuf.st_mode)) {
			RECURSIVELY_CALL_MVPROG_READ_DIR(NULL);
		}
#endif
		else
			fprintf(stdout, "%s\n", c_entry_name);
	}

	if (recursive_count != 0) {
		MVPROG_PERROR_RET(chdir(cwd), "chdir");
		recursive_count -= 1;
		if (closedir(dir) == -1) {
			perror("closedir");
		}
	}
	else {
		cwd = cwd != (char *)0 ? free(cwd), (char *)0 : (char *)0;
		if (closedir(dir) == -1)
			perror("closedir");
	}
}
MVPROG_INTTYPE main(int argc __attribute__((unused)), char **argv __attribute__((unused))) {
	int fd;
	ssize_t s __attribute__((unused)) = 0;
	char *p = 0;
	if (mkdir("newdir", S_IRWXU | S_IRGRP) && (errno != EEXIST)) {
		perror("");
		exit(-1);
	}
	if ((symlink("newdir", "symlink_newdir")) && (errno != EEXIST)) {
		perror("");
		exit(-1);
	}
	if ((fd = open("newdir", O_DIRECTORY, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH)) && (errno != EEXIST)){
		perror("open");
		return -1;
	}
	p = mvprog_malloc(p, 4, 0);
	//mvprog_read_dir(fd);
	//s = 4;
	/* test for null before assignment */
	mvprog_getcwd(p, &s);
	//puts(p);
	free(p);
	return MVPROG_SUCC;
}
