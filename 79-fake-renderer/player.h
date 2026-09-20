#ifndef PLAYER_H
#define PLAYER_H

#include "render_ops.h"

struct Player {
    int x;
    int y;
};

/* tile 0 = floor, 1 = wall, 2 = player (matches earlier print demos). */
void try_move(struct Player *p, int dx, int dy, int wall_ahead,
              const struct RenderOps *ops);

#endif
