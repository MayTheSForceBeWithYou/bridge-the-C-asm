#ifndef CLOCK_H
#define CLOCK_H

#include <time.h>

struct Clock {
    time_t (*now)(void);
};

time_t clock_real_now(void);
time_t clock_frozen_now(void);
void clock_freeze(time_t t);

/* Stamp uses whatever clock you pass — real in main, frozen in tests. */
time_t stamp_event(const struct Clock *clock);

#endif
