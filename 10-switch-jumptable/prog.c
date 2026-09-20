#include <stdio.h>
#include "../test/check.h"

int dense(int x)
{
    /* TODO: switch 0..7 returning distinct values; default -1 */
    (void)x;
    return -1;
}

int sparse(int x)
{
    /* TODO: switch on sparse cases; default -1 */
    (void)x;
    return -1;
}

int main(void)
{
    /* dense: x*x for 0..7 else -1; sparse: identity on {0,10,100,1000} else -1 */
    CHECK_EQ(dense(3), 9);
    CHECK_EQ(dense(9), -1);
    CHECK_EQ(sparse(100), 100);
    CHECK_EQ(sparse(50), -1);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
