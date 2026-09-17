# 22 — `add3` in asm, called from C (Track B)

## Goal
Hand-write a System V AMD64 function called from C.

## Contract
```c
long add3(long a, long b, long c);
```
- Args arrive in the registers you discovered in exercise 04.
- Return the sum in `%rax`.
- Preserve callee-saved registers if you use them (you should not need any).

## Do
1. Implement `add3` in `add3.s` (stub currently returns garbage via `ret` only).
2. `make && make run` — harness expects `add3(1,2,3) == 6` etc.

## Success
Harness prints `ok` and exits 0. Failure prints `FAIL` and exits non-zero.

## Refs
Discover ABI from your own `gcc -S` output (ex. 04–05). `man 1 as`, `man 1 gcc`,
`info as`.

Also in nvim-dap: make; open harness; break at call; F11 into `add3` — see `NVIM_DAP.md`.