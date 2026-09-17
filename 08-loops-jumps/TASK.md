# 08 — Loop jump shapes (Track A)

## Goal
Compare `for` / `while` / `do`-`while` control flow in asm.

## Do
1. Implement three functions that sum `1..n` (or fold an array) using each loop
   style exactly once.
2. `make asm` at `-O0`. Sketch the jump graph for each.

## Observe / answer
- Where is the loop test — top or bottom?
- How many conditional jumps vs unconditional jumps per loop?
- Does `-O2` reshape them into the same pattern?

## Refs
`man 1 gcc`, `man 1 objdump`
