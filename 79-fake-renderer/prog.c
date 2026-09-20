#include "../test/check.h"
#include "player.h"
#include "render_fake.h"

int main(void)
{
    struct RenderFake fake;
    struct Player p = { .x = 5, .y = 5 };

    render_fake_init(&fake);

    /* Walk into a wall: position must stay put, and the draw must show
       the old coordinates — the claim a real renderer cannot assert. */
    try_move(&p, 1, 0, 1 /* wall ahead */, &fake.ops);

    CHECK_EQ(p.x, 5);
    CHECK_EQ(p.y, 5);
    CHECK_EQ(fake.draw_count, 1);
    CHECK_EQ(fake.draws[0].x, 5);
    CHECK_EQ(fake.draws[0].y, 5);
    CHECK_EQ(fake.draws[0].tile, 2);

    /* Control: an open step moves and records the new cell. */
    render_fake_reset(&fake);
    try_move(&p, 1, 0, 0 /* open */, &fake.ops);
    CHECK_EQ(p.x, 6);
    CHECK_EQ(p.y, 5);
    CHECK_EQ(fake.draw_count, 1);
    CHECK_EQ(fake.draws[0].x, 6);

    return test_report();
}
