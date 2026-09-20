#include "lib.h"

unsigned rng_next(unsigned *state)
{
    *state = *state * 1664525u + 1013904223u;
    return *state;
}
