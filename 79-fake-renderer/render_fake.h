#ifndef RENDER_FAKE_H
#define RENDER_FAKE_H

#include "render_ops.h"

#define FAKE_MAX_DRAWS 64

struct FakeDraw {
    int x;
    int y;
    int tile;
};

struct RenderFake {
    struct RenderOps ops;
    struct FakeDraw draws[FAKE_MAX_DRAWS];
    int draw_count;
};

void render_fake_init(struct RenderFake *fake);
void render_fake_reset(struct RenderFake *fake);

#endif
