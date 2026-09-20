#include <stdio.h>
#include "../test/check.h"

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
    CHECK_EQ(sum_a(&a), 3);
    CHECK_EQ(sum_b(&b), 7);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
