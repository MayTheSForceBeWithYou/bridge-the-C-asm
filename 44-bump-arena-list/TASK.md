# 44 — bump arena linked list (Track D)

## Goal
`mmap` one arena, bump-allocate nodes, build a linked list — **no malloc/free**.

## Do
1. `mmap` a chunk (e.g. 64 KiB) RW anonymous.
2. Implement `arena_alloc(size)` that bumps a pointer (align as you like).
3. Rebuild exercise-17 style list: nodes with `val` + `next`, sum == 6 for
   values 1,2,3.
4. `munmap` at the end. Zero calls to `malloc`/`free` (check with `nm` /
   reading your source).

## Success
`./prog` prints `ok`. `nm ./prog` shows no `malloc`/`free` undefined syms
from your code (libc may still be linked for printf — that is OK here).

## Refs
`man 2 mmap`, `man 2 munmap`, `man 3 printf`
