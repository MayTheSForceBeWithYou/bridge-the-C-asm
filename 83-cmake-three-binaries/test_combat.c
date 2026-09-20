#include <stdio.h>

#include "../test/check.h"
#include "lib.h"

int main(void)
{
    /* Every one of these passes under either combat module. That is the
       trap: a green suite is not evidence that you linked the shipped one. */
    CHECK_EQ(combat_damage(10, 3), 7);
    CHECK_EQ(combat_damage(5, 5), 0);
    CHECK_EQ(config_answer(), 42);

    /* Not an assertion — a fingerprint. Compare this number with the one
       ./game prints for the same inputs. */
    printf("run_tests: combat_damage(3, 5) = %d\n", combat_damage(3, 5));

    return test_report();
}
