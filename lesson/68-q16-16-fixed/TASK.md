# 68 — Q16.16 fixed point (practice)

Read `LESSON.md` first. This file is only the lab.

## Implement

1. Q16.16 as `int32_t` (`1.0 -> 1<<16`).
2. `q_mul`, `q_div` with 64-bit intermediates + shifts.
3. Check `1.5 * 2.0 == 3.0` in Q16.16.
4. `make asm` — no XMM float arithmetic in your functions.

## Done when

- Harness arithmetic checks pass; asm is integer (`imul`/`sar` etc.); `ok`.

## Lookup

`man 1 gcc`, `man 1 objdump`, exercise 16.
