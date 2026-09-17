#include <stdio.h>
#include <stdlib.h>

long sum_to(long n)
{
    /* TODO: sum 1..n (0 if n<1) */
    (void)n;
    return 0;
}

int main(void)
{
    long got = sum_to(10);
    if (got != 55) {
        fprintf(stderr, "FAIL sum_to(10)=%ld want 55\n", got);
        exit(1);
    }
    puts("ok");
    return 0;
}
