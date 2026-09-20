#include "../test/check.h"
#include "clock.h"

int main(void)
{
    struct Clock real = { .now = clock_real_now };
    struct Clock frozen = { .now = clock_frozen_now };

    clock_freeze((time_t)1700000000);
    CHECK_EQ((int)stamp_event(&frozen), 1700000000);
    CHECK_EQ((int)stamp_event(&frozen), 1700000000);

    /* Real clock is wired for shipping; tests never call it. */
    (void)real;

    return test_report();
}
