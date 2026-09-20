#include <stdio.h>
#include "../test/check.h"

int max2(int a, int b)
{
    /* TODO */
    (void)a; (void)b;
    return 0;
}

int abs_int(int x)
{
    /* TODO */
    (void)x;
    return 0;
}

int main(void)
{
    CHECK_EQ(max2(3, 9), 9);
    CHECK_EQ(abs_int(-7), 7);
    printf("max2(3,9)=%d abs_int(-7)=%d\n", max2(3, 9), abs_int(-7));
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
