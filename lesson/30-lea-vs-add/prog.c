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

int *idx(int *p, long i)
{
    /* TODO: return &p[i] or p+i */
    (void)p; (void)i;
    return p;
}

long scale(long x, long y)
{
    /* TODO: return 8*x + y (or similar) */
    (void)x; (void)y;
    return 0;
}

long weird(long x)
{
    /* TODO: return 3*x + 1 */
    (void)x;
    return 0;
}

int main(void)
{
    int a[4] = {0, 1, 2, 3};
    expect_int("*idx(a,2)", *idx(a, 2), 2);
    expect_long("scale(2,3)", scale(2, 3), 19);
    expect_long("weird(5)", weird(5), 16);
    puts("ok");
    return 0;
}
