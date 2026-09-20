#include <stdio.h>
#include "../test/check.h"

long sum_for(long n)
{
    /* TODO: for-loop sum 1..n (define 0 if n<1) */
    (void)n;
    return 0;
}

long sum_while(long n)
{
    /* TODO: while-loop */
    (void)n;
    return 0;
}

long sum_do(long n)
{
    /* TODO: do-while — careful with n<1 */
    (void)n;
    return 0;
}

int main(void)
{
    CHECK_EQ(sum_for(10), 55);
    CHECK_EQ(sum_while(10), 55);
    CHECK_EQ(sum_do(10), 55);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
