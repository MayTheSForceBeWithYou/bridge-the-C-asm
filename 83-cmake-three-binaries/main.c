#include <stdio.h>

#include "lib.h"

int main(void)
{
    printf("damage=%d config=%d\n", combat_damage(10, 3), config_answer());
    return 0;
}
