# 27 — Asm calls C (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and run

1. In `apply_triple.s`, implement `long apply_triple(long x)` so it calls
   `c_triple` (provided in `harness.c`) and returns that result.
2. Set up args, align stack before `call`, save/restore callee-saved regs if
   you use them.
3. `make && make run`.

## Done when

- Harness prints `ok`.

## Lookup

`man 1 as`; Track A `-O0` `.s` around calls for save patterns.
