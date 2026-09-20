#include "../test/check.h"
#include "sys_ops.h"

int main(void)
{
    struct SysOps fake = {
        .write = sys_fake_write,
        .open = sys_fake_open,
    };

    /* Happy path: one write, three bytes. */
    sys_fake_reset();
    CHECK_EQ(greet(&fake, 1), 0);
    CHECK_EQ(sys_fake_write_count(), 1);
    CHECK_EQ(sys_fake_bytes_written(), 3); /* h i \n */

    /* The kernel refused. Reproducing this against a real fd means finding
       a broken pipe on purpose; the fake just says no. */
    sys_fake_reset();
    sys_fake_fail_next();
    CHECK_EQ(greet(&fake, 1), -1);
    CHECK_EQ(sys_fake_bytes_written(), 0);

    /* The kernel accepted part of it. write(2) is permitted to do this, and
       a greet that ignores the return value passes every other check here. */
    sys_fake_reset();
    sys_fake_short_next();
    CHECK_EQ(greet(&fake, 1), -1);
    CHECK_EQ(sys_fake_bytes_written(), 2);

    return test_report();
}
