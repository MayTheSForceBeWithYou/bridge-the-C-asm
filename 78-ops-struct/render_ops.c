#include <stdio.h>

#include "render_ops.h"

static void print_clear(void)
{
    puts("clear");
}

static void print_draw_tile(int x, int y, int tile)
{
    printf("draw_tile %d %d %d\n", x, y, tile);
}

static void print_present(void)
{
    puts("present");
}

void render_print_ops(struct RenderOps *ops)
{
    ops->clear = print_clear;
    ops->draw_tile = print_draw_tile;
    ops->present = print_present;
}

void draw_scene(const struct RenderOps *ops, int x, int y, int tile)
{
    ops->clear();
    ops->draw_tile(x, y, tile);
    ops->present();
}
