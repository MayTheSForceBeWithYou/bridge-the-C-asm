#include <stdio.h>
#include "../test/check.h"

int helper(int x, int y)
{
    /* TODO: nontrivial so regs get used; return x+y+1 */
    (void)x; (void)y;
    return 0;
}

int main(void)
{
    CHECK_EQ(helper(20, 21), 42);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
