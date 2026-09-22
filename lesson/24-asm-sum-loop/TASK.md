# 24 — Sum loop in asm (practice)

Read `LESSON.md` first — especially AT&T `cmp` decoding and fall-through.
This file is only the lab.

## Build and run

1. Implement `long sum_to(long n)` in `sum_to.s` with an explicit loop
   (`cmp` / `jcc` / `jmp` as needed). `sum_to(0) == 0`; tests use `n >= 0`.
2. `make && make run`.

## Done when

- Harness prints `ok`.
- You can point at your `cmp`/`jcc` pair and state the inequality it implements
  in AT&T terms.

## Lookup

`man 1 as`, `info as`; compare jump shapes to exercise 08.
