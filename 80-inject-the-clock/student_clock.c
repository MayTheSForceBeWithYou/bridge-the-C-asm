#include "clock.h"

/* Your version of cooldown.c. Build it instead of the reference with:
 *
 *     make clean && make SRCS="prog.c clock.c student_clock.c"
 *
 * The same prog.c drives both, so passing means your seam behaves. */

time_t stamp_event(const struct Clock *clock)
{
    /* TODO: return the current time according to this clock. */
    (void)clock;
    return 0;
}

int cooldown_ready(const struct Clock *clock, time_t started, time_t seconds)
{
    /* TODO: has `seconds` elapsed since `started`, per this clock? */
    (void)clock;
    (void)started;
    (void)seconds;
    return 0;
}
