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

long f1(long a);
long f2(long a, long b);
long f6(long a, long b, long c, long d, long e, long f);
long f7(long a, long b, long c, long d, long e, long f, long g);

long f1(long a)
{
    /* TODO */
    (void)a;
    return 0;
}

long f2(long a, long b)
{
    /* TODO */
    (void)a; (void)b;
    return 0;
}

long f6(long a, long b, long c, long d, long e, long f)
{
    /* TODO */
    (void)a; (void)b; (void)c; (void)d; (void)e; (void)f;
    return 0;
}

long f7(long a, long b, long c, long d, long e, long f, long g)
{
    /* TODO */
    (void)a; (void)b; (void)c; (void)d; (void)e; (void)f; (void)g;
    return 0;
}

int main(void)
{
    /* Sums: f1(1)=1, f2(1,2)=3, f6(1..6)=21, f7(1..7)=28 */
    expect_long("f1", f1(1), 1);
    expect_long("f2", f2(1, 2), 3);
    expect_long("f6", f6(1, 2, 3, 4, 5, 6), 21);
    expect_long("f7", f7(1, 2, 3, 4, 5, 6, 7), 28);
    puts("ok");
    return 0;
}
