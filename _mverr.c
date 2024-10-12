#include "_mv.h"

const char *mvErrorString = {
	"mv: mv requires two or more file operand\n",
	"mv: missing destination file operand\n",
	"mv: empty operand\n",
	"mv: invalid option"
	"mv: arguments exceeds MVFILE_MAX\n",
	"mv: argument %d is null or empty\n",
	"mv: insufficient memory\n",
	"mv: cannot rename file\n",
	"mv: cannot perform action -  permission denied\n",
	"mv: cannot perform action - '%s' does not exist\n"
};

__attribute__((noreturn)) void usageError(int err)
{
	char *n = mvErrorString[err];
	write(STDERR_FILENO, n, strlen(n));
	write (STDERR_FILENO, "Try 'mv --help' for more information.", 39);
	exit(EXIT_FAILURE);
}
__attribute__((noreturn)) void mvError(int err)
{
	char *n = mvErrorString[err];
	write(STDERR_FILENO, n, strlen(n));
	exit(EXIT_FAILURE);
}
void warnErrmsg(int err __attribute__((unused)))
{
}
