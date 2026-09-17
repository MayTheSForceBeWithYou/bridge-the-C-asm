# 23 — Stack scratch + 16-byte align (Track B)

## Goal
A function that needs local stack space and respects ABI alignment before a
`call`.

## Contract
```c
long scratch_sum(long a, long b);
```
- Allocate space for at least two local quads on the stack.
- Store `a` and `b` into those locals, then reload and return `a+b`.
- Before any `call` (if you call a helper — optional), `%rsp` ≡ 0 (mod 16).
  Even without a call, practice keeping the frame aligned the way a real
  prologue would (`push %rbp` already changes alignment — account for it).

## Do
1. Implement in `scratch.s`.
2. `make && make run`.

## Success
Harness prints `ok`.

## Refs
`man 1 as`, `info as`, discover prologue patterns from Track A `.s` files.

Also in nvim-dap: launch the harness binary; memory-view `$rsp` after entry — see `NVIM_DAP.md`.