# 07 — Arrays vs pointers (Track A)

## Goal
Check whether array indexing and pointer arithmetic produce the same asm.

## Do
1. Implement two functions that sum `n` ints:
   - `int sum_idx(const int *a, int n);` using `a[i]`
   - `int sum_ptr(const int *a, int n);` using pointer increment / `*p`
2. `make asm` at `-O0` and `-O2`. Diff the two functions' asm.

## Observe / answer
- At `-O0`, are the instruction sequences identical or merely equivalent?
- At `-O2`, do they converge?
- How is the scale factor for `int` visible (e.g. `leaq (,%reg,4)`)?

## Refs
`man 1 gcc`, `man 1 objdump`, `info as`

Also in nvim-dap: F9 on `sum_idx` / `sum_ptr`, memory-view the array — see `NVIM_DAP.md`.