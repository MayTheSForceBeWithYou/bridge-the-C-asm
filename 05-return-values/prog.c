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

struct Pair {
    int x;
    int y;
};

int ret_int(void)
{
    /* TODO */
    return 0;
}

long ret_long(void)
{
    /* TODO */
    return 0;
}

struct Pair ret_pair(void)
{
    struct Pair p;
    /* TODO: set p.x and p.y */
    p.x = 0;
    p.y = 0;
    return p;
}

int main(void)
{
    /* Contract: ret_int->42, ret_long->100, Pair{3,4} */
    expect_int("ret_int", ret_int(), 42);
    expect_long("ret_long", ret_long(), 100);
    struct Pair p = ret_pair();
    expect_int("pair.x", p.x, 3);
    expect_int("pair.y", p.y, 4);
    puts("ok");
    return 0;
}
