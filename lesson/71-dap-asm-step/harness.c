#include <stdio.h>
#include <stdlib.h>

/* Hand-written asm: return (a ^ b) + c */
long twiddle(long a, long b, long c);

static void expect(long a, long b, long c, long want)
{
    long got = twiddle(a, b, c);
    if (got != want) {
        fprintf(stderr, "FAIL twiddle(%ld,%ld,%ld)=%ld want %ld\n",
                a, b, c, got, want);
        exit(1);
    }
}

int main(void)
{
    expect(0x1, 0x2, 0x3, (0x1 ^ 0x2) + 0x3);
    expect(7, 7, 1, (7 ^ 7) + 1);
    expect(0, 0, 0, 0);
    puts("ok");
    return 0;
}
