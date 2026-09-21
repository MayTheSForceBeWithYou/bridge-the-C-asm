# 42 — brk heap growth (practice)

Read `LESSON.md` first. This file is only the lab.

## Implement

1. In `prog.c`, use `sbrk`/`brk` to query the break, grow by 4096, and pattern-check the new region.
2. Print old/new break addresses; printf is OK.
3. Print `ok` and exit 0 on success.
4. Optional: `command -v strace && strace -e brk,mmap ./prog` after also `malloc(1<<20)`.

## Done when

- `./prog` prints `ok` after grow + pattern check and exits 0.

## Lookup

`man 2 brk`, `man 2 sbrk`, `man 2 mmap`, `info libc`, `man 3 printf`.
