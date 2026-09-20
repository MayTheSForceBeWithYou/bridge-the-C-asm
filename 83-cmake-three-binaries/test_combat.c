#include <stdio.h>

#include "lib.h"

int main(void)
{
    if (combat_damage(5, 5) != 0) {
        return 1;
    }
    if (config_answer() != 42) {
        return 1;
    }
    puts("ok");
    return 0;
}
