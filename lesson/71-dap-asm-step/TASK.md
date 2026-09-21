# 71 — dap asm step (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. Implement `twiddle` in `twiddle.s`: `(a ^ b) + c` via System V regs. `make`
   → `./twiddle_test`; `make run` prints `ok`.
2. Open `harness.c`. **F9** on the `twiddle(...)` call inside `expect`.
3. `<leader>dc` → **"Launch existing executable"** → `./twiddle_test` (not
   "Build current file").
4. Note `a`,`b`,`c` via Scopes / `<leader>de`. **F11** into asm; inspect
   rdi/rsi/rdx/rax groups as you step. Optional `<leader>dm` if you spill.
5. `<leader>dt` when done. gdb alternative: `break twiddle`, `info registers`,
   `stepi`.

## Done when

- `./twiddle_test` prints `ok`.
- You stepped from harness into `.s` under nvim-dap (and/or gdb) and saw the
  ABI regs.

## Lookup

`NVIM_DAP.md`, `man 1 as`, `man 1 gdb`.
