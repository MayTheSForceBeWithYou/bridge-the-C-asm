#include <stdio.h>
#include "../test/check.h"

long f1(long a);
long f2(long a, long b);
long f6(long a, long b, long c, long d, long e, long f);
long f7(long a, long b, long c, long d, long e, long f, long g);

long f1(long a)
{
    /* TODO */
    (void)a;
    return 0;
}

long f2(long a, long b)
{
    /* TODO */
    (void)a; (void)b;
    return 0;
}

long f6(long a, long b, long c, long d, long e, long f)
{
    /* TODO */
    (void)a; (void)b; (void)c; (void)d; (void)e; (void)f;
    return 0;
}

long f7(long a, long b, long c, long d, long e, long f, long g)
{
    /* TODO */
    (void)a; (void)b; (void)c; (void)d; (void)e; (void)f; (void)g;
    return 0;
}

int main(void)
{
    /* Sums: f1(1)=1, f2(1,2)=3, f6(1..6)=21, f7(1..7)=28 */
    CHECK_EQ(f1(1), 1);
    CHECK_EQ(f2(1, 2), 3);
    CHECK_EQ(f6(1, 2, 3, 4, 5, 6), 21);
    CHECK_EQ(f7(1, 2, 3, 4, 5, 6, 7), 28);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
