#ifndef RENDER_OPS_H
#define RENDER_OPS_H

struct RenderOps {
    void (*clear)(void);
    void (*draw_tile)(int x, int y, int tile);
    void (*present)(void);
};

/* Fill ops with the print leaf's function pointers. */
void render_print_ops(struct RenderOps *ops);

/* Draw a one-tile "scene" through whatever leaf ops points at. */
void draw_scene(const struct RenderOps *ops, int x, int y, int tile);

#endif
