# 42 — brk heap growth (Track D)

## Goal
Grow the data break with `brk(2)`, use the new memory, contrast with libc
`malloc` (optional strace).

## Do
1. In `prog.c`, call `sbrk(0)` / `brk` (or syscall wrappers) to:
   - query current break
   - grow by 4096
   - write/read a pattern in the new region
2. Print old/new break addresses (printf OK here — this exercise is about
   the break, not nostdlib).
3. Optional: `command -v strace && strace -e brk,mmap ./prog` after also
   calling `malloc(1<<20)` once — note whether glibc used `brk` or `mmap`.
   Fallback without strace: read `man 2 brk` and `info libc` malloc notes.

## Success
`./prog` prints `ok` after a successful grow + pattern check. Exits 0.

## Refs
`man 2 brk`, `man 2 sbrk`, `man 2 mmap`, `info libc`, `man 3 printf`
