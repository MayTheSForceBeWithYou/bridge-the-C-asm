#include <stdio.h>
#include "../test/check.h"

long fac_naive(long n)
{
    /* TODO */
    (void)n;
    return 0;
}

long fac_tail(long n, long acc)
{
    /* TODO: tail-recursive */
    (void)n; (void)acc;
    return 0;
}

int main(void)
{
    CHECK_EQ((int)(fac_naive(5)), (int)(120));
    CHECK_EQ((int)(fac_tail(5, 1)), (int)(120));
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
