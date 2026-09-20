#include "clock.h"

/* Reference implementation of the code under test — the translation unit
   student_clock.c replaces. Note what is absent: no time(NULL), no global
   read. Every answer arrives through the clock parameter. */

time_t stamp_event(const struct Clock *clock)
{
    return clock->now();
}

int cooldown_ready(const struct Clock *clock, time_t started, time_t seconds)
{
    return clock->now() - started >= seconds;
}
