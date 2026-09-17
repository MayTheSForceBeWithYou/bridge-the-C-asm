# 33 — gdb asm add3 live (nvim-dap + gdb) (Track C)

## Goal
Watch your hand-written `add3` ABI live: args in regs, sum in `%rax`.

## nvim-dap (primary)

1. Implement `add3` in `add3.s` (sum three `long`s).
2. `make` → binary is `./add3_test` (`NAME` from Makefile).
3. Open `harness.c`. **F9** on the line that **calls** `add3` inside `expect`
   (call-site break is often clearer than breaking on the asm symbol first).
4. `<leader>dc` → **"Launch existing executable"** → `./add3_test`.
5. At the call site: `<leader>de` / Scopes on `a`,`b`,`c`. Then **F11** to step
   into asm.
6. In dap-ui Scopes, find register groups (CodeLLDB). Confirm the three integer
   args land where Track A/B taught you (rdi/rsi/rdx equivalents) before you
   overwrite them. Hover / Watches as needed.
7. DAP REPL (`<leader>dr`) is optional; prefer Scopes + `<leader>de` first.
8. Step / continue until the harness prints `ok`.

## gdb CLI (alternative)

```
gdb -q ./add3_test
(gdb) break add3
(gdb) run
(gdb) info registers rdi rsi rdx rax
(gdb) stepi
(gdb) info registers
(gdb) continue
(gdb) quit
```

Confirm args match `rdi`/`rsi`/`rdx` before you clobber them.

## Success
`./add3_test` prints `ok`. Live regs match the ABI from Track A/B.

## Refs
`NVIM_DAP.md`, `man 1 gdb`, `man 1 as`, gdb `help info registers`
