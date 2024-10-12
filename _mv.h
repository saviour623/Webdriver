#ifndef _MV_H
#define _MV_H
#include "shell_header.h"

#ifdef __linux__
#include <sys/fsuid.h>
#endif
#include <dirent.h>
#include <sys/vfs.h>
#include <stdarg.h>

#define IS_RDWRXTE(PERM) (PERM & (S_IRUSR | S_IWUSR | S_IXUSR))
#define PROC_OWNED(ENT, PROCID) (ENT.st_uid == PROCID)
#define BPERM_DIR S_IWUSR | S_IRUSR | S_IXUSR | S_IXGRP | S_IRGRP | S_IWGRP | S_IROTH | S_IXOTH
#define BPERM_FLE S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH

#define MVFILE_MAX 256
typedef struct pathsc {
	char *pth_parenpath;
	char *sc_abspath;
} pathsc;

typedef struct _mv_dest_info {
	char *ds_name;
	int ds_fd;
#ifdef _SYS_FS_H
	struct statfs *ds_fs;
#endif
} _mv_dest_info;
/* error numbers */
#define MVINCMPARG 1
#define MVINCMP2RAG 2
#define MVEMPTY 3
#define MVINVALOPT 4
#define MVOVERFLW 5
#define MVARGEMPT 6
#define MVNOMEM 7
#define MVRENAME 8
#define MVEEXIST 9
#define MVSTAT 10
#endif
