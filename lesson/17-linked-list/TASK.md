# 17 — Linked list pointer chasing (practice)

Read `LESSON.md` first — especially **How to decode the chase loads**. This file
is only the lab.

## Build and inspect

1. Define `struct Node { int val; struct Node *next; };`.
2. Implement `int list_sum(const struct Node *head);`.
3. Build a short stack-allocated list in `main`. `make asm` at `-O0` and `-O2`.

## Done when

- You identified load offsets for `val` vs `next` (with widths).
- You explained the NULL test in asm.
- You noted any `-O2` scheduling / shape differences.

## Lookup

`man 1 gcc`, `man 1 objdump`.
