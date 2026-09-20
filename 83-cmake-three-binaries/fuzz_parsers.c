#include <stdio.h>

#include "lib.h"

int main(void)
{
    unsigned s = 1;
    printf("fuzz %u\n", rng_next(&s));
    return 0;
}
