# 26 — `write` via raw syscall (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and run

Two acceptable shapes — pick one, or do both.

**C-callable (default `make`):** implement
`long my_write(int fd, const void *buf, unsigned long n)` in `my_write.s`
(`__NR_write` = 1; return kernel `%rax`).

**Freestanding:** `_start` in `write_n.s` writes a message to fd 1, then
`__NR_exit`. Use `make freestanding`.

1. Implement the syscall sequence.
2. Confirm stdout shows the message; harness exits 0.

## Done when

- Bytes appear on stdout and the chosen build's success check passes.

## Lookup

`man 2 write`, `man 2 syscall`, `man 2 syscalls`,
`/usr/include/asm/unistd_64.h`, `ausyscall`, `man 1 as`, `man 1 ld`.
