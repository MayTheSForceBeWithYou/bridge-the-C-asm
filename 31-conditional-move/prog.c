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

int max2(int a, int b)
{
    /* TODO */
    (void)a; (void)b;
    return 0;
}

int abs_int(int x)
{
    /* TODO */
    (void)x;
    return 0;
}

int main(void)
{
    expect_int("max2(3,9)", max2(3, 9), 9);
    expect_int("abs_int(-7)", abs_int(-7), 7);
    printf("max2(3,9)=%d abs_int(-7)=%d\n", max2(3, 9), abs_int(-7));
    puts("ok");
    return 0;
}
