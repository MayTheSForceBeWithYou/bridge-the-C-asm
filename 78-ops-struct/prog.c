#include <stdio.h>

#include "render_ops.h"

int main(void)
{
    struct RenderOps ops;
    render_print_ops(&ops);
    draw_scene(&ops, 3, 4, 2);
    puts("ok");
    return 0;
}
