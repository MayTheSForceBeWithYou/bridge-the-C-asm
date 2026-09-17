# 27 — Asm calls C (Track B)

## Goal
From asm, call a C function — callee-saved discipline + 16-byte align before
`call`.

## Contract
C provides:
```c
long c_triple(long x);  /* returns 3*x — already implemented in harness.c */
```
You implement in asm:
```c
long apply_triple(long x);  /* must call c_triple and return its result */
```

## Do
1. In `apply_triple.s`, set up args, align stack, `call c_triple`, return `%rax`.
2. If you use callee-saved registers, save/restore them.
3. `make && make run`.

## Success
Harness prints `ok`.

## Refs
`man 1 as`, discover caller/callee-saved patterns from Track A `-O0` `.s`
(look at which regs are pushed around calls). No ABI PDF.
