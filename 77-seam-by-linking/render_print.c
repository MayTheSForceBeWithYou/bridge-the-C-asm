#include <stdio.h>

#include "render.h"

/* Stand-in leaf: same six entry points as render_ncurses.c / render_sdl3.c.
   Swap this .c for another at link time — that is the seam. */

bool render_init(void)
{
    puts("render_init");
    return true;
}

void render_shutdown(void)
{
    puts("render_shutdown");
}

void render_clear(void)
{
    puts("render_clear");
}

void render_draw_tile(int x, int y, TileType t)
{
    printf("render_draw_tile %d %d %d\n", x, y, (int)t);
}

void render_draw_text(int x, int y, const char *s)
{
    printf("render_draw_text %d %d %s\n", x, y, s);
}

void render_present(void)
{
    puts("render_present");
}
