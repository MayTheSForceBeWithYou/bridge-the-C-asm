#include <stdio.h>
#include <stdlib.h>

long fac(long n);

static void expect(long n, long want)
{
    long got = fac(n);
    if (got != want) {
        fprintf(stderr, "FAIL fac(%ld)=%ld want %ld\n", n, got, want);
        exit(1);
    }
}

int main(void)
{
    expect(0, 1);
    expect(1, 1);
    expect(5, 120);
    expect(10, 3628800);
    puts("ok");
    return 0;
}
