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

int inc(int x)
{
    /* TODO */
    (void)x;
    return 0;
}

int dec(int x)
{
    /* TODO */
    (void)x;
    return 0;
}

int apply(int (*fp)(int), int x)
{
    /* TODO: return fp(x); */
    (void)fp; (void)x;
    return 0;
}

int main(void)
{
    expect_int("apply(inc,41)", apply(inc, 41), 42);
    expect_int("apply(dec,43)", apply(dec, 43), 42);
    puts("ok");
    return 0;
}
