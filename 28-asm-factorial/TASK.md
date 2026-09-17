# 28 — Recursive factorial in pure asm (Track B, stretch)

## Goal
Manual stack frames + recursive `call`.

## Contract
```c
long fac(long n);  /* n>=0; fac(0)=1 */
```
- Implement recursion in asm (call `fac` from itself).
- Save whatever you need across the recursive call.
- Return result in `%rax`.

## Do
1. Fill `fac.s`.
2. `make && make run`.

## Success
Harness prints `ok` for several `n`.

## Refs
Compare to exercises 13–14 (C recursion / tail calls). `man 1 as`, `info as`.
