#include <stdio.h>
#include "../test/check.h"

/* Tiny program for nvim-dap breakpoint / step practice.
 * Fill classify so the harness expectations hold; then debug it. */

int classify(int x)
{
    /* TODO:
     *  x < 0  -> -1
     *  x == 0 ->  0
     *  x > 0  ->  1
     */
    (void)x;
    return 0;
}

int accum(int n)
{
    /* TODO: return 0+1+...+(n-1); if n<=0 return 0 */
    (void)n;
    return 0;
}

int main(void)
{
    CHECK_EQ(classify(-3), -1);
    CHECK_EQ(classify(0), 0);
    CHECK_EQ(classify(7), 1);
    CHECK_EQ(accum(5), 10);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
