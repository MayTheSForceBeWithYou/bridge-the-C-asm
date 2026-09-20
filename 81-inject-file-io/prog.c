#include "../test/check.h"
#include "file_ops.h"

int main(void)
{
    struct FileOps fake = { .read_all = file_fake_read_all };

    file_fake_set(
        "# comment\n"
        "width = 80\n"
        "\n"
        "height=24\n"
    );

    CHECK_EQ(parse_config_count(&fake, "ignored.ini"), 2);

    file_fake_set("no_pairs_here\n");
    CHECK_EQ(parse_config_count(&fake, "ignored.ini"), 0);

    /* A failing read is a leaf behavior the real one would need a missing
       file to produce. The fake just plants NULL. */
    file_fake_set(NULL);
    CHECK_EQ(parse_config_count(&fake, "ignored.ini"), -1);

    return test_report();
}
