# 16 — Float / double in xmm* (Track A)

## Goal
Contrast FP argument/return registers with the integer convention you found
in exercises 04–05.

## Do
1. Implement:
   - `float addf(float a, float b);`
   - `double muld(double a, double b);`
   - a mixed function `double mix(double a, int n);`
2. `make asm disasm`.

## Observe / answer
- Which `xmm*` registers carry FP args / returns?
- How is the `int` mixed with a `double` arg — which GP register?
- Spot `addss`/`addsd`/`mulss`/`mulsd` (names vary; read your listing).

## Refs
`man 1 gcc`, `man 1 objdump` — discover FP ABI from output.
