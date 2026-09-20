#include <stdio.h>

#include "lib.h"

int main(void)
{
    unsigned s = 1;
    printf("fuzz: rng_next = %u\n", rng_next(&s));
    return 0;
}
