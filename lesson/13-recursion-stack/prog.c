#include <stdio.h>
#include <stdlib.h>


static void __attribute__((unused)) expect_long(const char *what, long got, long want)
{
    if (got != want) {
        fprintf(stderr, "FAIL %s: got %ld want %ld\n", what, got, want);
        exit(1);
    }
}

static void __attribute__((unused)) expect_int(const char *what, int got, int want)
{
    if (got != want) {
        fprintf(stderr, "FAIL %s: got %d want %d\n", what, got, want);
        exit(1);
    }
}

long fac(long n)
{
    /* TODO: recursive factorial; define fac(0)=1; reject n<0 as you like */
    (void)n;
    return 0;
}

int main(void)
{
    expect_long("fac(5)", fac(5), 120);
    expect_long("fac(0)", fac(0), 1);
    puts("ok");
    return 0;
}
