#include <stdio.h>

void boom(void)
{
    volatile int *p = NULL;
    /* intentional NULL deref for addr2line / bt practice */
    printf("about to crash: %d\n", *p);
}

int main(void)
{
    boom();
    return 0;
}
