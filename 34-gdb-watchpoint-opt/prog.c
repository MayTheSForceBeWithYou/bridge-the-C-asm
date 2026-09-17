#include <stdio.h>
#include <stdlib.h>

int bump(int n)
{
    int x = 0;
    int i;
    for (i = 0; i < n; i++)
        x += 1;
    return x;
}

int main(void)
{
    int got = bump(5);
    if (got != 5) {
        fprintf(stderr, "FAIL bump: %d\n", got);
        exit(1);
    }
    puts("ok");
    return 0;
}
