#include <stdio.h>
#include "../test/check.h"

long add2_c(long a, long b)
{
    /* TODO: plain C add */
    (void)a; (void)b;
    return 0;
}

long add2_asm(long a, long b)
{
    long sum;
    /* TODO: asm volatile Extended Asm adding a and b into sum */
    sum = 0;
    (void)a; (void)b;
    return sum;
}

int main(void)
{
    CHECK_EQ(add2_c(20, 22), 42);
    CHECK_EQ(add2_asm(20, 22), 42);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
