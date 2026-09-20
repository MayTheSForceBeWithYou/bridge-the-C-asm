#include <stdio.h>
#include "../test/check.h"

long fac(long n)
{
    /* TODO: recursive factorial; define fac(0)=1; reject n<0 as you like */
    (void)n;
    return 0;
}

int main(void)
{
    CHECK_EQ((int)(fac(5)), (int)(120));
    CHECK_EQ((int)(fac(0)), (int)(1));
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
