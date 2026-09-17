# 19 — Inline asm bridge (Track A)

## Goal
Write a tiny `asm volatile` fragment that is still callable from C.

## Do
1. Implement `long add2_asm(long a, long b);` using Extended Asm that adds
   the two operands and returns the sum (output operand → C return).
2. Keep a pure-C `add2_c` for comparison. Print both.
3. Browse: `info gcc` → search for Extended Asm (or from the top node, dig
   into C Extensions). Also `man 1 gcc`.

## Observe / answer
- What constraint letters did you use for inputs/outputs? Why?
- Does `asm volatile` still get reordered relative to nearby code at `-O2`?
  Experiment with a nearby store to a global.
- Compare the surrounding compiler-generated prologue to exercise 02.

## Refs
`man 1 gcc`, `info gcc` (Extended Asm), `man 1 objdump`
