#include <stdio.h>

#include "../test/check.h"
#include "render_ops.h"

/* A second leaf, defined right here in the harness. It records instead of
   printing, which is the whole reason the ops struct exists: two
   implementations live in this one process at the same time. Exercise 79
   grows this idea into a proper fake. */

static int calls[3];
static int ncalls;
static int last_x, last_y, last_tile;

static void count_clear(void)   { if (ncalls < 3) calls[ncalls++] = 1; }
static void count_present(void) { if (ncalls < 3) calls[ncalls++] = 3; }

static void count_draw_tile(int x, int y, int tile)
{
    if (ncalls < 3) {
        calls[ncalls++] = 2;
    }
    last_x = x;
    last_y = y;
    last_tile = tile;
}

int main(void)
{
    struct RenderOps ops;

    /* The print leaf, so you can watch the calls go by. */
    render_print_ops(&ops);
    CHECK(ops.clear != NULL);
    CHECK(ops.draw_tile != NULL);
    CHECK(ops.present != NULL);
    draw_scene(&ops, 3, 4, 2);

    /* The counting leaf, so the harness can assert what draw_scene did
       rather than trusting the output you just read. */
    struct RenderOps counting = {
        .clear = count_clear,
        .draw_tile = count_draw_tile,
        .present = count_present,
    };
    draw_scene(&counting, 7, 8, 4);

    CHECK_EQ(ncalls, 3);
    CHECK_EQ(calls[0], 1); /* clear */
    CHECK_EQ(calls[1], 2); /* draw_tile */
    CHECK_EQ(calls[2], 3); /* present */
    CHECK_EQ(last_x, 7);
    CHECK_EQ(last_y, 8);
    CHECK_EQ(last_tile, 4);

    if (test_report() != 0) {
        return 1;
    }
    puts("ok");
    return 0;
}
