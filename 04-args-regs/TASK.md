# 04 — Argument registers (Track A)

## Goal
Discover the System V AMD64 integer argument-passing convention from compiler
output — there is no ABI man page in the verified set.

## Do
1. Implement:
   - `long f1(long a);`
   - `long f2(long a, long b);`
   - `long f6(long a,b,c,d,e,f);`  (six args)
   - `long f7(long a,b,c,d,e,f,g);` (seven args)
   Each should return a simple mix of its arguments (sum is fine).
2. Call each from `main` with distinct constants. `make asm disasm`.

## Observe / answer
- Which registers hold args 1..6? (Read the `.s` / disassembly — do not look up
  an ABI PDF.)
- Where does the 7th argument live?
- Where does the return value appear for the caller?

## Refs
`man 1 gcc`, `man 1 objdump` — discover ABI from output only.

Also in nvim-dap: break in `f6`/`f7`, inspect arg registers in Scopes — see `NVIM_DAP.md`.