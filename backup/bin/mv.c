#include "_mv.h"

int _cptfs_flag;
#define TST_LOGFUN(tst, err, act)\
	do { if ((tst) == (err)){ act; exit(EXIT_FAILURE); } } while(0)

void mvRenameFile(char *__restrict__ oldpath, char *__restrict__ newpath)
{
	if (rename(oldpath, newpath) == -1)
		mvError(MVRENAME);
}
void *mvAlllocBl(size_t n)
{
	void *newblk = malloc(newblk, size);
	if (newblk == NULL)
		mvError(MVNOMEM);
	return newblk;
}
char *path_ncpy(char *dest, const char *src, size_t len)
{
	register int c, oo = 0;

	if (src == NULL)
	{
		errno = EINVAL;
		return NULL;
	}
	if (len == -1)
		len = strlen(src);
	if (dest == NULL && *src != 0)
	{
		dest = malloc(len * sizeof (char));
		if (dest == NULL)
		{
			errno = ENOMEM;
			return NULL;
		}
	}
	for (; len && (c = *src); len--, *src++)
	{
		if ((c == '/') && (*(src + 1) == '/'))
			continue;
		dest[oo++] = *src;
	}
	return dest;
}
__attribute__((nonnull)) static char *get_ppath(char *path)
{
	static char paren_path[PATH_MAX];
	register int oo = 0, n_bkslash = 0;
	register size_t len = strlen(path), tmlen = len;

	if (len > PATH_MAX || len == 0 || *path == 0)
		return NULL;

	for (; (tmlen > 0) && (n_bkslash = (path[tmlen - 1] != '/')); tmlen--, oo++)
		;
	if (n_bkslash)
		return ((paren_path[0] = '.'), (paren_path[1] = '/'), (paren_path[2] = 0), paren_path);
	return path_ncpy(paren_path, path, len - oo);
}
gid_t sgrplist(gid_t _sgid)
{
	gid_t grp[NGROUPS_MAX + 1];
	register int oo, ngrps = getgroups(NGROUPS_MAX + 1, grp), atgrp = 0;

	if (ngrps == -1)
		return (_sgid);
	for (oo = 0; oo < ngrps; oo++)
	{
		atgrp = grp[oo];
		if (_sgid == atgrp)
			return (atgrp);
	}
	return (_sgid);
}
int faccesswx(int fd, char *fileName)
{
	struct stat statbuf;
	mode_t file_mode, s_perm = 0;
	register uid_t euid_proc = geteuid(), fuid_proc __UNUSED__;
	register gid_t egid_proc = getegid(), fgid_proc __UNUSED__;
	register int flag_id;

	TST_LOGFUN(fstat(fd, &statbuf), -1, perror("fstat"));

	/* for a regular file, it's modification loosely depends on the ownership of its parent dir */
	if (S_ISREG(statbuf.st_mode))
		TST_LOGFUN(stat(get_ppath(fileName), &statbuf), -1, perror("stat"));
	file_mode = statbuf.st_mode;

#if defined(__linux__) && defined(_SYS_FSUID_H)
	fuid_proc = setfsuid(-1);
	fgid_proc = setfsgid(-1);

	if (fuid_proc == 0 || fuid_proc == statbuf.st_uid)
		flag_id = 1;
	else if (fgid_proc == statbuf.st_gid || sgrplist(statbuf.st_gid) == statbuf.st_gid)
		flag_id = 2;
	if (flag_id != 0)
		goto fsperm;
#endif
	/* file must be owned by a user or group(s) */
	if (euid_proc == 0 || euid_proc == statbuf.st_uid)
		flag_id = 1;
	else if (egid_proc == statbuf.st_uid || sgrplist(statbuf.st_gid) == statbuf.st_gid)
		flag_id = 2;
	else
		return (s_perm & 0);
fsperm:
	if (S_ISDIR(statbuf.st_mode))
	{
		/* if sticky bit is set on the dir and the dir doesnt belong to the process, then we dont have access */
		if ((file_mode & S_ISVTX) && !(PROC_OWNED(statbuf, euid_proc))) /* TODO: compare against fsuid */
			return (s_perm & 0);
	}

	/* we are only interested in the execute and write permission */
	if (euid_proc != 0)
		s_perm = flag_id == 1 ? (S_IWUSR | S_IXUSR) & file_mode
			: flag_id == 2 || (sgrplist(statbuf.st_gid) != egid_proc)
			? (S_IWGRP | S_IXGRP) & file_mode : (S_IWOTH | S_IXOTH) & file_mode;

	else
		s_perm = 1;
	return s_perm;
}
char *_get_current_dir_name(void)
{
	char *bf = malloc(PATH_MAX);
	if (bf == NULL)
		return NULL;
}
int _mvMultiFiles(int numOfiles, char **flelist)
{
}
/* if */
int _mvTwoFiles(char *__restrict__ flesrc, char *__restrict__ fledest)
{
	struct stat statSrc, statDest;
	struct statfs fsSrc, fsDest;
	register int fd, creatFlag = 0;
	char *fletmp = NULL;

	if (flesrc == NULL || *flesrc == 0 || fledest == NULL || *fledest == 0)
		usageError(MVARGEMPT);
	if (stat(flesrc, &statSrc) == -1)
		mvError(MVEEXIST);
	/* if the destination does not exist, it's likely we create one. A direct call to rename() should create a corresponding file, however, we can't be certain if it exist on the same file system so as to copy rather name renaming */
	if (stat(fledest, &statDest) == -1 && (errno == ENOENT))
	{
		if (S_ISDIR(statSrc.st_mode))
		{
			fd = open(fledest, O_CREAT | O_DIRECTORY | O_EXCL | O_RDWR, BPERM_DIR);
			if (fd == -1 && errno != EEXIST)
			{
				/* handle error */
				exit(-1);
			}
		}
		else if (S_ISREG(statSrc.st_mode))
		{
			fd = open(fledest, O_CREAT | O_EXCL | O_RDWR, BPERM_FLE);
			if (fd == -1 && errno != EEXIST)
			{
				/* handle error */
				exit(-1);
			}
		}
		creatFlag = 1; /* let's remember we created a destination file */
	}
	else
	{
		/* handle error */
		exit(-1);
	}
	/* if the source file is a directory and the destination is otherwise, mv cannot be possible */
	if (S_ISDIR(statSrc.st_mode) && !(S_ISDIR(statDest.st_mode)))
	{
		/* handle error */
		exit(-1);
	}
	if (statfs(flesrc, &fsSrc) != -1 && statfs(fledest, &fsDest) != -1)
	{
		/* same fle system */
		if (fsSrc.f_type == fsDest.f_type)
		{
			if (creatFlag || !(statDest.st_size) || !(S_ISDIR(statDest.st_mode)))
				mvRenameFile(flesrc, fle);
			else
			{
				register int srclen = strlen(flesrc), destlen = strlen(flesrc);

				fletmp = mvAllocBlk(sizeof (char) * (srclen + destlen));
				path_ncpy(fletmp, fledest, destlen);
				if (fledest[destlen - 1] != '/')
					fletmp[destlen - 1] = '/';
				path_ncpy(fletmp + destlen, flesrc, srclen);
				mvRenameFile(flsrc, fletmp);
			}
		}
	}
	else /* copy */
	{
	}
}
void mv_directory_func(int fln, ...)
{
	va_list flelist;
	int fd, file_mode, permis[2];
	char *_mv_nputfile[MVFILE_MAX], *ptrfl;;
	__fsword_t tpfl[MVFILE_MAX];
	char *_mv_destination_fl;

	char *a = NULL, *b = NULL;
	struct stat statbuf;
	struct statfs fsbuf;
	register int oo;

	va_start(flelist, fln);

	switch (fln)
	{
	case 0:
		usageError(MVINCMPARG);
	case 1:
	    usageError(MVINCMP2ARG);
	default:
		if (fln > MVFILE_MAX)
			mvError(MVOVERFLW);
	}
	oo = 0;
	while (oo < fln)
	{
		ptrfl = (char *) va_arg(flelist, void *);
		if (ptrfl == NULL || *ptrfl == 0)
			warnErr(MVARGEMPT);
		_mv_nputfile[oo] = ptrfl;
		oo++;
	}
	_mv_destination_fl = _mv_nputfile[fln - 1];

	if (fln < 3)
		_mvTwoFiles(_mv_nputfile[0], _mv_nputfile[1]);
	else
		_mvMultiFiles(fln, _mv_nputfile);
}

int main(void)
{
	struct stat statbuf;
	//stat("./new", &statbuf);
	//perror("stat");
//	char path[PATH_MAX];
//	char *p = cpy_ppath("hop");
//	if (p != NULL)
//		puts(p);
//	puts(path_ncpy(NULL, "/hello/hi/////kl//p/power", 26));
//	printf("%s\n", realpath("/h/b/t/y", path));
	mv_directory_func(2, "./empty_test/new1", "./empty_test/new2");
	return (0);
}
	/* also change timestamp with utimes in mv*/
/* add umask builtin */
