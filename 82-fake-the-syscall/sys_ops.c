#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "sys_ops.h"

ssize_t sys_real_write(int fd, const void *buf, size_t n)
{
    return write(fd, buf, n);
}

int sys_real_open(const char *path, int flags, int mode)
{
    return open(path, flags, mode);
}

static int g_writes;
static size_t g_bytes;

void sys_fake_reset(void)
{
    g_writes = 0;
    g_bytes = 0;
}

int sys_fake_write_count(void)
{
    return g_writes;
}

size_t sys_fake_bytes_written(void)
{
    return g_bytes;
}

ssize_t sys_fake_write(int fd, const void *buf, size_t n)
{
    (void)fd;
    (void)buf;
    g_writes++;
    g_bytes += n;
    return (ssize_t)n;
}

int sys_fake_open(const char *path, int flags, int mode)
{
    (void)path;
    (void)flags;
    (void)mode;
    return 3; /* pretend fd */
}

int greet(const struct SysOps *ops, int fd)
{
    const char *msg = "hi\n";
    ssize_t n = ops->write(fd, msg, strlen(msg));
    return n == (ssize_t)strlen(msg) ? 0 : -1;
}
