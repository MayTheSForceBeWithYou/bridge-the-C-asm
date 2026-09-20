#ifndef SYS_OPS_H
#define SYS_OPS_H

#include <sys/types.h>

/* The seam, one level below the file reader in exercise 81: these two
   signatures are the kernel entry points, not libc conveniences. */
struct SysOps {
    ssize_t (*write)(int fd, const void *buf, size_t n);
    int (*open)(const char *path, int flags, int mode);
};

/* Real leaves — thin wrappers over libc, which is a thin wrapper over the
   syscall instruction you wrote by hand in exercises 26 and 40. */
ssize_t sys_real_write(int fd, const void *buf, size_t n);
int sys_real_open(const char *path, int flags, int mode);

/* Fake leaves — count and return, never enter the kernel. */
ssize_t sys_fake_write(int fd, const void *buf, size_t n);
int sys_fake_open(const char *path, int flags, int mode);

void sys_fake_reset(void);
int sys_fake_write_count(void);
size_t sys_fake_bytes_written(void);

/* Failure injection. sys_fake_fail_next() makes the next write return -1;
   sys_fake_short_next() makes it report one byte fewer than asked. Both are
   states a pipe-capture test cannot produce on demand. */
void sys_fake_fail_next(void);
void sys_fake_short_next(void);

/* Code under test: writes "hi\n" through ops->write. Returns 0 on a full
   write, -1 otherwise. */
int greet(const struct SysOps *ops, int fd);

#endif
