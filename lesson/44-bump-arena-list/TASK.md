# 44 — bump arena linked list (practice)

Read `LESSON.md` first. This file is only the lab.

## Implement

1. `mmap` a RW anonymous chunk (e.g. 64 KiB).
2. Implement `arena_alloc(size)` that bumps (align as you like).
3. Build a list with values 1,2,3; sum must be 6 — no `malloc`/`free` for nodes.
4. `munmap` at the end. Print `ok`.

## Done when

- `./prog` prints `ok`.
- Your code path allocates nodes only from the arena (`nm`/source check).

## Lookup

`man 2 mmap`, `man 2 munmap`, `man 3 printf`.
