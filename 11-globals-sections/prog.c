#include <stdio.h>
#include "../test/check.h"

/* TODO: add g (initialized), h (uninit), file-scope static, const global. */

int tick(void)
{
    /* TODO: local static counter */
    return 0;
}

int main(void)
{
    /* tick: local static starting at 0, return then increment */
    CHECK_EQ(tick(), 0);
    CHECK_EQ(tick(), 1);
    CHECK_EQ(tick(), 2);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
