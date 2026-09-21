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

/* TODO: add g (initialized), h (uninit), file-scope static, const global. */

int tick(void)
{
    /* TODO: local static counter */
    return 0;
}

int main(void)
{
    /* tick: local static starting at 0, return then increment */
    expect_int("tick#1", tick(), 0);
    expect_int("tick#2", tick(), 1);
    expect_int("tick#3", tick(), 2);
    puts("ok");
    return 0;
}
