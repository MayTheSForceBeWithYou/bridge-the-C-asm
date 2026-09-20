#ifndef SYS_OPS_H
#define SYS_OPS_H

#include <sys/types.h>

struct SysOps {
    ssize_t (*write)(int fd, const void *buf, size_t n);
    int (*open)(const char *path, int flags, int mode);
};

ssize_t sys_real_write(int fd, const void *buf, size_t n);
int sys_real_open(const char *path, int flags, int mode);

ssize_t sys_fake_write(int fd, const void *buf, size_t n);
int sys_fake_open(const char *path, int flags, int mode);

void sys_fake_reset(void);
int sys_fake_write_count(void);
size_t sys_fake_bytes_written(void);

/* Writes a greeting through ops->write. */
int greet(const struct SysOps *ops, int fd);

#endif
