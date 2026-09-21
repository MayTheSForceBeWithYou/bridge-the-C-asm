# 19 — Inline asm bridge (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and inspect

1. Implement `long add2_asm(long a, long b);` with Extended Asm that adds and
   returns the sum. Keep pure-C `add2_c` for comparison. Print both.
2. Browse `info gcc` Extended Asm for constraint spellings once you know what
   you need.
3. Inspect prologue around the fragment (compare to lesson 02). At `-O2`,
   experiment with a nearby store to a global and see what reorders.

## Done when

- You can name the constraint letters you used and why.
- You observed whether `asm volatile` still allowed reordering relative to a
  nearby global store at `-O2`.
- You compared the surrounding compiler-generated prologue to exercise 02.

## Lookup

`man 1 gcc`, `info gcc` (Extended Asm), `man 1 objdump`.
