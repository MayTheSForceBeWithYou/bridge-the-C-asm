#include "render_fake.h"

/* Reference fake — linked by the Makefile. Records draws in a static
   target set by render_fake_init. Student rewrite lives in render_fake.c. */

static struct RenderFake *g_fake;

static void fake_clear(void)
{
    /* Recording fake: clear does not erase history; tests call reset. */
}

static void fake_draw_tile(int x, int y, int tile)
{
    if (g_fake == 0) {
        return;
    }
    if (g_fake->draw_count >= FAKE_MAX_DRAWS) {
        return;
    }
    g_fake->draws[g_fake->draw_count].x = x;
    g_fake->draws[g_fake->draw_count].y = y;
    g_fake->draws[g_fake->draw_count].tile = tile;
    g_fake->draw_count++;
}

static void fake_present(void)
{
}

void render_fake_reset(struct RenderFake *fake)
{
    fake->draw_count = 0;
}

void render_fake_init(struct RenderFake *fake)
{
    fake->ops.clear = fake_clear;
    fake->ops.draw_tile = fake_draw_tile;
    fake->ops.present = fake_present;
    render_fake_reset(fake);
    g_fake = fake;
}
