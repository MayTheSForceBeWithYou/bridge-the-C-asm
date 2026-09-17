# 31 — Conditional move (Track A micro-rep)

## Goal
See `cmov*` vs branching for simple conditionals under optimization.

## Do
1. Implement `int max(int a, int b);` and `int abs_int(int x);` in straightforward C.
2. Compare `make asm` vs `make O=2 asm`.

## Observe / answer
- At `-O2`, does GCC emit `cmov*` / other branchless forms?
- At `-O0`, what do you get instead?

## Refs
`man 1 gcc`, `man 1 objdump`, `info as`
