# 09 — Signed vs unsigned compares (Track A)

## Goal
See `jl`/`jg` vs `jb`/`ja` (and friends) for signed vs unsigned.

## Do
1. Implement:
   - `int cmp_signed(int a, int b);` → negative / zero / positive style result
   - `int cmp_unsigned(unsigned a, unsigned b);` → same idea
2. Also write a loop that exits when an index hits a limit — once with `int`,
   once with `unsigned`. Prefer values near the top of the range so the
   difference matters conceptually.
3. `make asm`.

## Observe / answer
- Which conditional-jump mnemonics appear for signed vs unsigned?
- After `cmp`, what flags do those jumps consult? (Infer from names + `info as`
  or `gdb help` / experimentation.)

## Refs
`man 1 objdump`, `info as`, `man 1 gcc`
