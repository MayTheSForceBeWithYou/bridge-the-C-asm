#include "player.h"

enum { TILE_FLOOR = 0, TILE_WALL = 1, TILE_PLAYER = 2 };

void try_move(struct Player *p, int dx, int dy, int wall_ahead,
              const struct RenderOps *ops)
{
    int nx = p->x + dx;
    int ny = p->y + dy;

    if (!wall_ahead) {
        p->x = nx;
        p->y = ny;
    }

    ops->clear();
    ops->draw_tile(p->x, p->y, TILE_PLAYER);
    ops->present();
}
