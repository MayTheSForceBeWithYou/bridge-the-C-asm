#include <stdio.h>
#include "../test/check.h"

int *idx(int *p, long i)
{
    /* TODO: return &p[i] or p+i */
    (void)p; (void)i;
    return p;
}

long scale(long x, long y)
{
    /* TODO: return 8*x + y (or similar) */
    (void)x; (void)y;
    return 0;
}

long weird(long x)
{
    /* TODO: return 3*x + 1 */
    (void)x;
    return 0;
}

int main(void)
{
    int a[4] = {0, 1, 2, 3};
    CHECK_EQ(*idx(a, 2), 2);
    CHECK_EQ((int)(scale(2, 3)), (int)(19));
    CHECK_EQ((int)(weird(5)), (int)(16));
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
