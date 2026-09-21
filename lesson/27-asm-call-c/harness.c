#include <stdio.h>
#include <stdlib.h>

long c_triple(long x)
{
    return 3 * x;
}

long apply_triple(long x);

int main(void)
{
    long g = apply_triple(14);
    if (g != 42) {
        fprintf(stderr, "FAIL got %ld\n", g);
        return 1;
    }
    g = apply_triple(0);
    if (g != 0) {
        fprintf(stderr, "FAIL got %ld\n", g);
        return 1;
    }
    puts("ok");
    return 0;
}
