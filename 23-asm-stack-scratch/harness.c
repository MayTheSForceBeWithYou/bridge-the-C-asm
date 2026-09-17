#include <stdio.h>
#include <stdlib.h>

long scratch_sum(long a, long b);

int main(void)
{
    long g = scratch_sum(20, 22);
    if (g != 42) {
        fprintf(stderr, "FAIL got %ld\n", g);
        return 1;
    }
    g = scratch_sum(-5, 5);
    if (g != 0) {
        fprintf(stderr, "FAIL got %ld\n", g);
        return 1;
    }
    puts("ok");
    return 0;
}
