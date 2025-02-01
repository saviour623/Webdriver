#include <stdio.h>
int main (void)
{
    int *ptr;
    int *mptr[1];

    *mptr = (int[]){25};
    ptr = *mptr;
    *mptr = (int[]){30};

    printf("main - %d ptr - %d", *(*mptr), *ptr);
}
