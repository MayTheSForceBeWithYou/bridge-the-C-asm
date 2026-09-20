#include "render_fake.h"

/* Your version of render_fake_ref.c. Build it instead of the reference:
 *
 *     make clean && make SRCS="prog.c player.c render_fake.c"
 *
 * The ops wiring below is exercise 78's content and is done for you. What
 * is left is the part that makes this a fake rather than a stub: keeping
 * the draws where a CHECK_EQ can see them. */

/* RenderOps takes no userdata, so the leaf needs a file-scope target. See
   the lesson for why that narrowing is acceptable here and not in general. */
static struct RenderFake *g_fake;

static void fake_clear(void)
{
    /* Deliberately empty: a recording fake must not erase history, or the
       test loses the evidence it came for. Tests call render_fake_reset. */
}

static void fake_draw_tile(int x, int y, int tile)
{
    /* TODO: append {x, y, tile} to g_fake->draws and bump draw_count.
       Two guards to get right: g_fake may be unset, and draws[] holds at
       most FAKE_MAX_DRAWS entries. */
    (void)x;
    (void)y;
    (void)tile;
}

static void fake_present(void)
{
    /* Nothing to flush — there is no surface. */
}

void render_fake_reset(struct RenderFake *fake)
{
    /* TODO: forget the recorded draws without unwiring the ops. */
    (void)fake;
}

void render_fake_init(struct RenderFake *fake)
{
    fake->ops.clear = fake_clear;
    fake->ops.draw_tile = fake_draw_tile;
    fake->ops.present = fake_present;
    render_fake_reset(fake);
    g_fake = fake;
}
