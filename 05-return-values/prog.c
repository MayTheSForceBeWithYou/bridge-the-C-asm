#include <stdio.h>
#include "../test/check.h"

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
    CHECK_EQ(ret_int(), 42);
    CHECK_EQ((int)(ret_long()), (int)(100));
    struct Pair p = ret_pair();
    CHECK_EQ(p.x, 3);
    CHECK_EQ(p.y, 4);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
