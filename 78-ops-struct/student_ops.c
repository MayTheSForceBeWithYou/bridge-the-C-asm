#include <stdio.h>

#include "render_ops.h"

/* Practice stub — not linked by the Makefile. Fill this in, then point
   SRCS at it instead of render_ops.c if you want to prove your version. */

void render_print_ops(struct RenderOps *ops)
{
    /* TODO: assign clear / draw_tile / present to print leaf functions */
    (void)ops;
}

void draw_scene(const struct RenderOps *ops, int x, int y, int tile)
{
    /* TODO: call ops->clear, ops->draw_tile, ops->present */
    (void)ops;
    (void)x;
    (void)y;
    (void)tile;
}
