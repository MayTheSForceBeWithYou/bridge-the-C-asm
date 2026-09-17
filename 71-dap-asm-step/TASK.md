# 71 — dap asm step (Track C / dap)

## Goal
`make` a harness + tiny `.s`, launch the **existing** multi-file binary in
nvim-dap, break at the call, step into asm, observe registers / memory.

## Do
1. Implement `twiddle` in `twiddle.s`: return `(a ^ b) + c` (System V regs).
2. `make` → `./twiddle_test`. Confirm `make run` prints `ok`.
3. Open `harness.c`. **F9** on the `twiddle(...)` call inside `expect`.
4. `<leader>dc` → **"Launch existing executable"** → `./twiddle_test`
   (not "Build current file" — that would miss `twiddle.s`).
5. At the call: note `a`,`b`,`c` via Scopes / `<leader>de`.
6. **F11** step into asm. Inspect register groups in Scopes for rdi/rsi/rdx/rax
   equivalents as you step (F10 may skip coarse chunks — use F11 / continue).
7. Optional: `<leader>dm` if you spill anything to the stack (this stub need not).
8. `<leader>dt` when done.

## gdb CLI (alternative)

```
gdb -q ./twiddle_test
(gdb) break twiddle
(gdb) run
(gdb) info registers rdi rsi rdx rax
(gdb) stepi
(gdb) info registers
(gdb) continue
(gdb) quit
```

## Success
`./twiddle_test` prints `ok`. You stepped from harness into `.s` under nvim-dap
(and/or gdb) and saw the ABI regs.

## Refs
`NVIM_DAP.md`, `man 1 as`, `man 1 gdb`
