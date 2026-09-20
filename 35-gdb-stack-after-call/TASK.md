# 35 — gdb stack after call (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. Implement `scratch` in `scratch.s`: push three regs, pop them, return 42.
   `make` → `./stack_test` prints `ok`.
2. nvim-dap: **F9** at the call into `scratch`; launch `./stack_test`; **F11**
   in; note SP; **`<leader>dm`** at the stack pointer.
3. gdb (for per-push precision): break `call_site`, `run`, `stepi`, `x/8xg
   $rsp`; enter `scratch`, `stepi` across pushes, `info registers rsp`, dump
   again.

## Done when

- `./stack_test` prints `ok`.
- You can explain the 8-byte slots at `$rsp` (dap memory and/or `x/8xg`),
  including the return address and the per-push `%rsp` delta.

## Lookup

`NVIM_DAP.md`, `man 1 gdb`, gdb `help x`, `man 1 as`.
