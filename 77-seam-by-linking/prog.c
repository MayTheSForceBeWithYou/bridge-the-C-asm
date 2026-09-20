#include <stdio.h>

#include "render.h"

int main(void)
{
    if (!render_init()) {
        return 1;
    }
    render_clear();
    render_draw_tile(1, 2, TILE_PLAYER);
    render_draw_text(0, 0, "ok");
    render_present();
    render_shutdown();
    puts("ok");
    return 0;
}
