# 17 — Linked list pointer chasing (Track A)

## Goal
See pointer-chasing loads in asm.

## Do
1. Define a simple `struct Node { int val; struct Node *next; };`
2. Implement `int list_sum(const struct Node *head);`
3. Build a short stack-allocated list in `main`. `make asm` at `-O0` and `-O2`.

## Observe / answer
- What load offsets fetch `val` vs `next`?
- How does the loop test for NULL?
- At `-O2`, are loads scheduled differently?

## Refs
`man 1 gcc`, `man 1 objdump`
