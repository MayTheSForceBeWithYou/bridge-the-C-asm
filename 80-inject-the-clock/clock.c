#include "clock.h"

/* The two leaves. Only clock_real_now touches the wall clock, and nothing
   but a composition root should ever select it. */

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
