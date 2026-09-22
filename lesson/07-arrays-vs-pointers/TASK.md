# 07 — Arrays vs pointers (practice)

Read `LESSON.md` first — especially **What indexing means in asm** and **The
number-4 trap**. This file is only the lab.

## Build and inspect

1. Implement `sum_idx` (use `a[i]`) and `sum_ptr` (pointer increment / `*p`) for
   summing `n` ints.
2. `make asm` at `-O0` and `-O2` (copy `.s` aside between builds). Diff the two
   functions' asm.
3. Note scale factors and whether the shapes converge.

## Done when

- You stated whether `-O0` sequences were identical or merely equivalent.
- You found evidence of stride 4 for `int` (and labeled other `4`s correctly).
- You recorded what `-O2` did to the pair.

## Lookup

`man 1 gcc`, `man 1 objdump`, `info as`.
