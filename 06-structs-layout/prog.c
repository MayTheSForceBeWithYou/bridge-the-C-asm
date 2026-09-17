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

/* TODO: define structs A and B with different field orders. */

struct A {
    /* TODO */
    char c;
    int i;
};

struct B {
    /* TODO */
    int i;
    char c;
};

int sum_a(const struct A *p)
{
    /* TODO: use every field */
    (void)p;
    return 0;
}

int sum_b(const struct B *p)
{
    /* TODO */
    (void)p;
    return 0;
}

int main(void)
{
    struct A a = { .c = 1, .i = 2 };
    struct B b = { .i = 3, .c = 4 };
    printf("sizeof A=%zu B=%zu\n", sizeof a, sizeof b);
    expect_int("sum_a", sum_a(&a), 3);
    expect_int("sum_b", sum_b(&b), 7);
    puts("ok");
    return 0;
}
