#include <stdio.h>
#include <stdlib.h>

long sum_to(long n);

static void expect(long n, long want)
{
    long got = sum_to(n);
    if (got != want) {
        fprintf(stderr, "FAIL sum_to(%ld)=%ld want %ld\n", n, got, want);
        exit(1);
    }
}

int main(void)
{
    expect(0, 0);
    expect(1, 1);
    expect(10, 55);
    expect(100, 5050);
    puts("ok");
    return 0;
}
