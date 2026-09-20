#include <stdio.h>
#include "../test/check.h"

int cmp_signed(int a, int b)
{
    /* TODO: return -1 / 0 / 1 */
    (void)a; (void)b;
    return 0;
}

int cmp_unsigned(unsigned a, unsigned b)
{
    /* TODO: return -1 / 0 / 1 */
    (void)a; (void)b;
    return 0;
}

int main(void)
{
    CHECK_EQ(cmp_signed(-1, 1), -1);
    CHECK_EQ(cmp_signed(1, 1), 0);
    CHECK_EQ(cmp_signed(2, 1), 1);
    CHECK_EQ(cmp_unsigned(0u, 1u), -1);
    CHECK_EQ(cmp_unsigned(1u, 1u), 0);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
