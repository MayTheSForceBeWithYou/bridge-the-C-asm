#include "clock.h"

static time_t frozen;

time_t clock_real_now(void)
{
    return time(NULL);
}

time_t clock_frozen_now(void)
{
    return frozen;
}

void clock_freeze(time_t t)
{
    frozen = t;
}

time_t stamp_event(const struct Clock *clock)
{
    return clock->now();
}
