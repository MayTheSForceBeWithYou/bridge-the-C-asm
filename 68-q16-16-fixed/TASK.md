# 68 — Q16.16 fixed point (Track G)

## Goal
Multiply/divide Q16.16 values with integer shifts only — no XMM float ops.
Contrast with exercise 16.

## Do
1. Represent Q16.16 as `int32_t` (`1.0 -> 1<<16`).
2. Implement `q_mul`, `q_div` using 64-bit intermediates + shifts.
3. Check `1.5 * 2.0 == 3.0` in Q16.16.
4. `make asm` — confirm no `xmm` float arithmetic in your functions.

## Success
Harness arithmetic checks pass; asm is integer (`imul`/`sar` etc.). `ok`.

## Refs
`man 1 gcc`, `man 1 objdump`, exercise 16
