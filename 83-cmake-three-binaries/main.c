#include <stdio.h>

#include "lib.h"

int main(void)
{
    printf("game:      combat_damage(3, 5) = %d\n", combat_damage(3, 5));
    printf("game:      config_answer()     = %d\n", config_answer());
    return 0;
}
