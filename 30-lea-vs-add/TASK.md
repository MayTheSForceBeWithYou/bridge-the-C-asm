# 30 — LEA vs ADD (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and inspect

1. Write functions such as `p + i` for `int *p`, and integer forms like `3*x+1`
   and `8*x+y`.
2. `make asm` at `-O0` and `-O2`.
3. Note where `lea` appears instead of `imul`/`add`.

## Done when

- You pointed at `lea` used for address math and/or pure arithmetic.
- You answered whether `lea` is "always about addresses" with evidence.
- You noted `-O0` vs `-O2` differences for at least one expression.

## Lookup

`man 1 gcc`, `man 1 objdump`, `info as`.
