#include "../test/check.h"
#include "sys_ops.h"

int main(void)
{
    struct SysOps fake = {
        .write = sys_fake_write,
        .open = sys_fake_open,
    };

    sys_fake_reset();
    CHECK_EQ(greet(&fake, 1), 0);
    CHECK_EQ(sys_fake_write_count(), 1);
    CHECK_EQ((int)sys_fake_bytes_written(), 3); /* h i \n */

    return test_report();
}
