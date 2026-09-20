#ifndef RENDER_OPS_H
#define RENDER_OPS_H

struct RenderOps {
    void (*clear)(void);
    void (*draw_tile)(int x, int y, int tile);
    void (*present)(void);
};

#endif
