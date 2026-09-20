#include "sys_ops.h"

/* Your version of greet.c. Build it instead of the reference with:
 *
 *     make clean && make SRCS="prog.c sys_ops.c student_greet.c"
 *
 * The same prog.c drives both. Note that two of its checks fail only if you
 * ignore what ops->write returned. */

int greet(const struct SysOps *ops, int fd)
{
    /* TODO: write "hi\n" through ops->write on fd. Return 0 only if the
       write reported every byte; -1 on failure or a short write. */
    (void)ops;
    (void)fd;
    return -1;
}
