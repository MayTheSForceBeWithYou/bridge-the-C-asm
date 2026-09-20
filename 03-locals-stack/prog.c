#include <stdio.h>
#include "../test/check.h"

int work(void)
{
    /* TODO: several locals; make sure each is used so -O0 keeps them. */
    return 0;
}

int main(void)
{
    /* Contract: work() returns sum of locals a=1,b=2,c=3,arr={4,5,6,7} => 28.
       Harness exits 0 / prints ok when behavior is correct. */
    CHECK_EQ(work(), 28);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
