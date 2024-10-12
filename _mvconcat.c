#include "_mv.h"

char *mvPathConcat(char *strp[])
{
	int k = 0, i = 0, f = 0, g;
	static char bf[PATH_MAX];

	if (strp == NULL || *strp == NULL)
		return (char *)0;
	while ((strp[k++] != NULL))
		;
	printf("%d\n", k);
	for (; i < k; i++)
	{
		g = strlen(strp[i]);
		//strcat(bf, strp[i]);
		f += g;
		bf[f] = 0;
	}
	bf[f] = 0;
	//puts(bf);
}
int main(void)
{
	mvPathConcat((char *[]){"hello", "hi", NULL});
}
