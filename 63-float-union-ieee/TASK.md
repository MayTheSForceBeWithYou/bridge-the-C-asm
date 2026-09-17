# 63 — float union IEEE-754 (Track G)

## Goal
Type-pun a `float` through a `union` with `uint32_t`. **TRACE first on paper**
before running: predict sign/exponent/mantissa bits.

## Do
1. On paper: for `float f = 1.0f` (and `-2.5f`), write expected 32-bit layout.
2. Implement union pun; print hex bits.
3. Compare to your prediction; decode fields by hand.

## Success
Printed hex matches your paper prediction for at least `1.0f` and `-2.5f`.
Harness prints `ok` when hex equals documented expected constants.

## Refs
`man 1 gcc`, `man 1 objdump` (optional asm peek)
