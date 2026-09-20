#ifndef CLOCK_H
#define CLOCK_H

#include <time.h>

/* The seam: one function pointer standing where time(NULL) used to be. */
struct Clock {
    time_t (*now)(void);
};

/* Leaves. Production fills .now with clock_real_now; tests use the frozen
   pair below. */
time_t clock_real_now(void);
time_t clock_frozen_now(void);
void clock_freeze(time_t t);

/* Code under test. Neither function names `time` — they only call through
   the clock they are handed. */
time_t stamp_event(const struct Clock *clock);
int cooldown_ready(const struct Clock *clock, time_t started, time_t seconds);

#endif
