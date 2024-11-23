#include <ftw.h>
#include "mvdef.h"
#define _GNU_SOURCE
#define S_DOTREF_2DOTREF(s)						\
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

MVPROG_STR mvprog_getcwd (MVPROG_STR buf, ssize_t *bufsz) {
    MVPROG_STR gtcwd = NULL;
    MVPROG_STR cwd;
    MVPROG_STR *env_ptr;
    int8_t is_null = (buf == NULL);
    register ssize_t len = 0;

    if (!bufsz || ((*bufsz < 0) && !is_null) || (*bufsz >= MVPROG_MAX_MEM_ALLOC)) {
	/* bufsz must be set correctly to the size allocated to buf */
	fprintf(stderr, "mvpg>mvprog_getcwd: invalid buffer size");
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
	    cwd = buf;
    re:
	errno = 0;
	if ((getcwd(cwd, PATH_MAX) == NULL)) {
	    if (errno == ERANGE) {
		cwd = (MVPROG_STR)mvprog_malloc(buf, (len = *bufsz + MVPROG_FILENMAX), 1);
		if (cwd == NULL)
		    return (MVPROG_STR)0;

		*bufsz = len;
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
	len = strlen(gtcwd);

	if (is_null) {
	    cwd = (MVPROG_STR)mvprog_malloc(NULL, len + 1, 0);

	    if (cwd == NULL)
		return (MVPROG_STR)0;
	    *bufsz = len;
	}
	else if (len > *bufsz) {
	    cwd = mvprog_malloc(buf, len + 1, 1);
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
    buf = cwd;
    return cwd;
}
#define MVPROG_BKSZ 1024
#define MVPROG_RDFL_ROUT 0xff
#define MVPROG_RDFL_RIN 0xfe
#define MVPROG_SYMCURR_DIR "."
#define MVPROG_SYMPREV_DIR ".."

typedef int __mvprog_fdt;
#define mvprog_rdpath_macro(dir) mvprog_rdpath(dir, 0, 0, NULL)
static inline char * __attribute ((always_inline)) mvprog_rdpath(char *dir) {
    DIR *PDIR;
    struct dirent *dir_entry;
    register char *crr_entry, *proc_cwd;
    register ssize_t dir_crr_entry;
    register size_t prog_stat = 1;
    register size_t rddir_stat = 0;
    ssize_t bufsz = 1024;
    char *mem = malloc(1024);
    DIR *bkfd[MVPROG_BKSZ];
    char *gcwdbf[4062], *initcwd;

    if ((dir == NULL) || (*dir == 0)) {
	return NULL;
    }
    PDIR = opendir(dir);
    if (PDIR == NULL) {
	perror("mvprog>opendir");
	return NULL;
    }
    errno = 0;
    while ((dir_entry = readdir(PDIR)) || rddir_stat) {
	if (dir_entry == NULL) {
	    PDIR = bkfd[--rddir_stat];
	    dir = prog_stat ? (--prog_stat, MVPROG_SYMCURR_DIR) : mvprog_getcwd(mem, &bufsz);
	    continue;
	}
	crr_entry = dir_entry->d_name;
	
	if (S_DOTREF_2DOTREF(crr_entry)) {
	    continue;
	}
	//some fs implementations provide a d_type in struct dirent
#if defined(_BSD_SOURCE) || defined(_DIRENT_HAVE_D_TYPE)
	dir_crr_entry = dir_entry->d_type == DT_DIR;
#else
#endif
	if (dir_crr_entry) {
	    //printf("%d %s %s\n", rddir_stat, getcwd(NULL, 0), dir);
	    if (chdir(dir)) {
		perror("chdir");
		return NULL;
	    }
	    bkfd[rddir_stat] = PDIR;
	    //	    puts(gcwdbf[rddir_stat]);

	    if (!(PDIR = opendir(crr_entry))) {
		if (errno == EACCES){
		    chdir(gcwdbf[rddir_stat - 1]);
		    PDIR = bkfd[rddir_stat--];
		    //rddir_stat--;
		    continue;
		}
		//rddir_stat += 1;
		//puts(getcwd(NULL, 0));
		//puts(dir);
		perror(crr_entry);
		return NULL;
	    }
	    dir = crr_entry;
	    rddir_stat += 1;
	}
	else {
	    puts(crr_entry);
	}
    }
}
/*
  root - Mvpg - .git - hook
  root - Mvpg (.git)

  Mvpg - .git (.hook) (0 - Mvpg) prog_stat: 1 rddir_stat: 1
  .git - hook (end) (1 - .git) prog_stat: 0 rddir_stat: 2 - 1
  .git - ref (end) (1 - .git) prog_stat: 0 rddir_stat: 2

  Mvdir - .git - hook
  Mvdir - .git - _hook_
 */
int nothing() {
    
}
MVPROG_INTTYPE main(int argc __attribute__((unused)), char **argv __attribute__((unused))) {
    int fd;
    /*if (mkdir("newdir", S_IRWXU | S_IRGRP) && (errno != EEXIST)) {
      perror("");
      exit(-1);
      }
      if ((symlink("newdir", "symlink_newdir")) && (errno != EEXIST)) {
      perror("");
      exit(-1);
      }*/
    //mvprog_rdpath("/");
    //ftw("/", nothing, 32000);
    //ssize_t sz = 0;
    //char *p = mvprog_getcwd(NULL, &sz);
    //char *p = getcwd(NULL, 0);
    //puts(p ? p : "null");
    //char *pd = malloc(8);
    //pd = realloc(pd, 2);
    return MVPROG_SUCC;
}
