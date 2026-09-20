# 33 — gdb asm add3 live (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. Implement `add3` in `add3.s` (sum three `long`s → `%rax`). `make` →
   `./add3_test` prints `ok`.
2. Open `harness.c`. **F9** on the line that **calls** `add3` inside `expect`.
3. `<leader>dc` → **"Launch existing executable"** → `./add3_test`.
4. At the call: Scopes / `<leader>de` on `a`,`b`,`c`. **F11** into asm; confirm
   `%rdi`/`%rsi`/`%rdx` before you clobber them; finish with sum in `%rax`.
5. gdb alternative: `break add3`, `run`, `info registers rdi rsi rdx rax`,
   `stepi` as needed.

## Done when

- `./add3_test` prints `ok`.
- Live regs matched the Track A/B ABI at entry (nvim-dap and/or gdb).

## Lookup

`NVIM_DAP.md`, `man 1 gdb`, `man 1 as`, gdb `help info registers`.
