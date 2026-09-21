#include <stdio.h>
#include <stdlib.h>

long add3(long a, long b, long c);

static void expect(long a, long b, long c, long want)
{
    long got = add3(a, b, c);
    if (got != want) {
        fprintf(stderr, "FAIL add3(%ld,%ld,%ld)=%ld want %ld\n",
                a, b, c, got, want);
        exit(1);
    }
}

int main(void)
{
    expect(1, 2, 3, 6);
    expect(10, -3, 1, 8);
    expect(0, 0, 0, 0);
    puts("ok");
    return 0;
}
