#include "../test/check.h"
#include "clock.h"

int main(void)
{
    struct Clock frozen = { .now = clock_frozen_now };
    struct Clock real = { .now = clock_real_now };

    /* A frozen clock is stable: the same stamp twice, no tolerance window. */
    clock_freeze((time_t)1700000000);
    CHECK_EQ(stamp_event(&frozen), 1700000000);
    CHECK_EQ(stamp_event(&frozen), 1700000000);

    /* A 30-second cooldown started 10 seconds ago has not expired. */
    CHECK_EQ(cooldown_ready(&frozen, (time_t)1699999990, 30), 0);

    /* Advance the clock instead of sleeping. If cooldown_ready had cached a
       timestamp, or reached for time(NULL) itself, this check would fail. */
    clock_freeze((time_t)1700000025);
    CHECK_EQ(cooldown_ready(&frozen, (time_t)1699999990, 30), 1);
    CHECK_EQ(stamp_event(&frozen), 1700000025);

    /* Both leaves are live in this process at once. stamp_event answers
       differently per clock, which is the property "assert it is roughly
       now" could never establish. */
    CHECK(stamp_event(&real) != stamp_event(&frozen));

    return test_report();
}
