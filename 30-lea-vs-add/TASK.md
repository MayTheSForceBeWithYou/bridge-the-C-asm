# 30 — LEA vs ADD (Track A micro-rep)

## Goal
See when GCC prefers `lea` for address math / scaled adds.

## Do
1. Write functions such as:
   - `p + i` for `int *p`
   - `3*x + 1`, `8*x + y` style arithmetic on integers
2. `make asm` at `-O0` and `-O2`.

## Observe / answer
- Where does `lea` show up instead of `imul`/`add`?
- Is `lea` always about addresses, or also about arithmetic?

## Refs
`man 1 gcc`, `man 1 objdump`, `info as`
