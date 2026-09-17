# 18 — `volatile` vs optimization (Track A)

## Goal
Watch `-O2` delete non-volatile work that it must keep for `volatile`.

## Do
1. Implement two functions that do a busy-looking store/load loop:
   - one with an ordinary `int`
   - one with `volatile int`
2. `make O=2 asm disasm` and compare.

## Observe / answer
- Which loads/stores survive for the volatile version?
- Did the non-volatile loop collapse to a constant or disappear?
- Why might that matter for MMIO-style code? (Think; no need for a man page.)

## Refs
`man 1 gcc`, `info gcc`, `man 1 objdump`
