# 14 — Tail-call optimization? (Track A)

## Goal
See whether `-O2` turns a tail-recursive factorial into a jump.

## Do
1. Implement two versions:
   - `long fac_naive(long n);` — classic `n * fac(n-1)`
   - `long fac_tail(long n, long acc);` — tail-recursive helper
2. Compare `make asm` vs `make O=2 asm`.

## Observe / answer
- Does `fac_tail` become a loop (`jmp` to itself) at `-O2`?
- Does `fac_naive`? Why / why not (look at the work after the recursive call)?
- Confirm with `objdump -d` on both builds.

## Refs
`man 1 gcc`, `man 1 objdump`, `info gcc`
