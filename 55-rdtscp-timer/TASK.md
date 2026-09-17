# 55 — rdtscp cycle timer (Track F)

## Goal
Hand-roll a cycle counter with **`rdtscp`** (not plain `rdtsc`) and time a
simple loop. Relative comparisons only — absolute counts vary.

This CPU has `rdtscp` + `constant_tsc` (verified).

## Do
1. Implement `rdtscp_now()` via inline asm Extended Asm (or tiny `.s`).
2. Time a loop of ~10M increments; print elapsed cycles.
3. Run thrice — note variance. Never treat one number as "correct".

## Success
`./prog` prints three positive cycle deltas and `ok`.

## Refs
`man 1 gcc` (Extended Asm), `info gcc`, exercise 19
