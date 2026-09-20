#include "render_fake.h"

/* TODO: student fake — record every draw_tile into fake->draws[].
   The linked binary uses render_fake_ref.c instead. When you are ready,
   point SRCS at this file and drop the _ref one. */

static struct RenderFake *g_fake;

static void fake_clear(void)
{
    /* TODO */
}

static void fake_draw_tile(int x, int y, int tile)
{
    /* TODO: append {x,y,tile} if room remains */
    (void)x;
    (void)y;
    (void)tile;
}

static void fake_present(void)
{
    /* TODO */
}

void render_fake_reset(struct RenderFake *fake)
{
    /* TODO */
    (void)fake;
}

void render_fake_init(struct RenderFake *fake)
{
    /* TODO: fill fake->ops, reset counts, set g_fake */
    (void)fake;
    (void)g_fake;
}
