#include <string.h>

#include "sys_ops.h"

/* Reference implementation of the code under test — the translation unit
   student_greet.c replaces. The short-write check is the reason this is
   worth testing at all: write(2) is allowed to move fewer bytes than asked,
   and code that ignores the return value is wrong in a way no successful
   run reveals. */

int greet(const struct SysOps *ops, int fd)
{
    const char *msg = "hi\n";
    size_t len = strlen(msg);
    ssize_t n = ops->write(fd, msg, len);

    return (n == (ssize_t)len) ? 0 : -1;
}
