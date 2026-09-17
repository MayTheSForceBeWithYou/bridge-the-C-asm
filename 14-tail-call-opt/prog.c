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

long fac_naive(long n)
{
    /* TODO */
    (void)n;
    return 0;
}

long fac_tail(long n, long acc)
{
    /* TODO: tail-recursive */
    (void)n; (void)acc;
    return 0;
}

int main(void)
{
    expect_long("fac_naive(5)", fac_naive(5), 120);
    expect_long("fac_tail(5,1)", fac_tail(5, 1), 120);
    puts("ok");
    return 0;
}
