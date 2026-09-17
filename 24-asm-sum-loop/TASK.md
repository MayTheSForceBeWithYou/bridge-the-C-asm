# 24 — Sum loop in asm (Track B)

## Goal
Labels + conditional jumps, no C control flow inside the routine.

## Contract
```c
long sum_to(long n);  /* sum of 1..n; sum_to(0)==0; n>=0 assumed for tests */
```

## Do
1. Implement with an explicit loop (`cmp`/`jcc`/`jmp` as needed) in `sum_to.s`.
2. `make && make run`.

## Success
Harness prints `ok`.

## Refs
`man 1 as`, `info as`, compare jump shapes to exercise 08.
