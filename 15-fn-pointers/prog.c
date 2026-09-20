#include <stdio.h>
#include "../test/check.h"

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
    CHECK_EQ(apply(inc, 41), 42);
    CHECK_EQ(apply(dec, 43), 42);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
