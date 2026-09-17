# 26 — `write` via raw syscall (Track B)

## Goal
Issue `__NR_write` yourself.

## Contract (two acceptable shapes — pick one, or do both)

**Freestanding:** `_start` writes a message to fd 1, then `__NR_exit`.

**Callable from C:**
```c
long my_write(int fd, const void *buf, unsigned long n);
/* returns syscall return value (bytes written or -errno-style — match
   what the kernel returns in %rax; harness checks bytes written >= 0) */
```
- `__NR_write` = 1 — confirm in `unistd_64.h` / `ausyscall write`
- Args: fd, buf, count in the syscall arg registers (`man 2 syscall`)

Starter ships both `write_n.s` (freestanding) and `my_write.s` + harness.
Default `make` builds the C-callable test. Use `make freestanding` for `_start`.

## Do
1. Implement the syscall sequence.
2. Run and confirm output appears on stdout; harness exits 0.

## Refs
`man 2 write`, `man 2 syscall`, `man 2 syscalls`,
`/usr/include/asm/unistd_64.h`, `ausyscall`, `man 1 as`, `man 1 ld`
