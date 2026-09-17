# 29 — Call clobbers (Track A micro-rep)

## Goal
Which registers does a `call` smash from the caller's point of view?

## Do
1. Write `main` that puts distinctive constants into several registers via
   a small `static inline` asm blob *or* by calling a carefully written
   helper — simplest path: write two C functions and inspect `-O0` asm
   around a `call`, noting which regs the caller reloads after return.
2. Alternatively: in gdb, `break` before/after a `call`, `info registers`.

## Observe / answer
- List registers that are not preserved across the call (caller-saved /
  clobbered).
- List registers the callee must restore if used (callee-saved).
- Derive this from *your* listing / gdb session, not from memory of an ABI doc.

## Refs
`man 1 gcc`, `man 1 objdump`, `man 1 gdb`, gdb `help info registers`
