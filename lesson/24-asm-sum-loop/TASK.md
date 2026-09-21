# 24 — Sum loop in asm (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and run

1. Implement `long sum_to(long n)` in `sum_to.s` with an explicit loop
   (`cmp` / `jcc` / `jmp` as needed). `sum_to(0) == 0`; tests use `n >= 0`.
2. `make && make run`.

## Done when

- Harness prints `ok`.

## Lookup

`man 1 as`, `info as`; compare jump shapes to exercise 08.
