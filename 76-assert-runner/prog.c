#include "../test/check.h"

/* Demo harness: one deliberate failure so exit status and file:line are
   visible. Your own macros go in my_check.h — see TASK.md. */

int main(void)
{
    CHECK(1 + 1 == 2);
    CHECK_EQ(2 + 2, 4);
    CHECK_STR("ok", "ok");
    CHECK_PTR((void *)0, (void *)0);

    /* Deliberate failure — proves the report path. */
    CHECK_EQ(1 + 1, 3);

    return test_report();
}
