# 05 — Return values (Track A)

## Goal
See where `int` / `long` / small-struct returns land.

## Do
1. Implement:
   - `int ret_int(void);` → some constant
   - `long ret_long(void);` → some constant
   - `struct Pair { int x; int y; }; struct Pair ret_pair(void);`
2. Call them from `main`, print results. `make asm` at `-O0`.

## Observe / answer
- For `ret_int` / `ret_long`, which register holds the return at the `ret`?
- For `ret_pair`, how does the compiler return the struct? (Look at both the
  callee and the caller.) Does it use registers, a hidden pointer, or both?

## Refs
`man 1 gcc`, `man 1 objdump` — discover from output.
