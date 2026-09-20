#include <fcntl.h>
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
static int g_fail_next;
static int g_short_next;

void sys_fake_reset(void)
{
    g_writes = 0;
    g_bytes = 0;
    g_fail_next = 0;
    g_short_next = 0;
}

int sys_fake_write_count(void)
{
    return g_writes;
}

size_t sys_fake_bytes_written(void)
{
    return g_bytes;
}

void sys_fake_fail_next(void)
{
    g_fail_next = 1;
}

void sys_fake_short_next(void)
{
    g_short_next = 1;
}

ssize_t sys_fake_write(int fd, const void *buf, size_t n)
{
    (void)fd;
    (void)buf;
    g_writes++;

    if (g_fail_next) {
        g_fail_next = 0;
        return -1;
    }
    if (g_short_next && n > 0) {
        g_short_next = 0;
        g_bytes += n - 1;
        return (ssize_t)(n - 1);
    }

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
