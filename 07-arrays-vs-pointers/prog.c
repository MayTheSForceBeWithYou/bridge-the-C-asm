#include <stdio.h>
#include "../test/check.h"

int sum_idx(const int *a, int n)
{
    /* TODO: sum with a[i] */
    (void)a; (void)n;
    return 0;
}

int sum_ptr(const int *a, int n)
{
    /* TODO: sum with pointer walking */
    (void)a; (void)n;
    return 0;
}

int main(void)
{
    int a[] = {1, 2, 3, 4, 5};
    CHECK_EQ(sum_idx(a, 5), 15);
    CHECK_EQ(sum_ptr(a, 5), 15);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
