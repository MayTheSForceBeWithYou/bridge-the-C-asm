#include "lib.h"

/* Shipped combat rule: damage never goes below zero. This is the module
   `game` links. Whether `run_tests` links it is the whole exercise. */

int combat_damage(int atk, int def)
{
    int d = atk - def;
    return d > 0 ? d : 0;
}
