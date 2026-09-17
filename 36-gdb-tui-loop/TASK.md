# 36 — gdb TUI loop (nvim-dap + gdb) (Track C)

## Goal
Watch a register / accumulator increment through a loop. gdb TUI
(`layout regs` / `layout asm`) is one view; dap-ui + virtual text is another.

## nvim-dap (primary in-editor)

1. Implement `sum_to` so `sum_to(10)==55`.
2. `make`. Open `prog.c`. **F9** on `sum_to`.
3. `<leader>dc` → Launch existing → `./prog` (or **"Launch ./prog"**).
4. Step the loop with **F10** / **F11**. Watch **virtual text** and **Scopes**
   update each iteration.
5. Optional: Watches on the accumulator / `n`. Compare the feel to gdb TUI —
   dap-ui is not a 1:1 `layout regs` clone; both show live state.

## gdb CLI / TUI (alternative)

```
gdb -q ./prog
(gdb) break sum_to
(gdb) run
(gdb) layout regs
(gdb) layout asm
(gdb) stepi
(gdb) stepi
(gdb) stepi
(gdb) stepi
(gdb) stepi
(gdb) stepi
(gdb) continue
(gdb) quit
```

(If TUI glitches: `tui disable` / `tui enable`, or use `layout split`.)

Watch the accumulator register change each iteration (`stepi` = instruction).

## Success
`./prog` prints `ok`. You stepped the loop body in nvim-dap and/or TUI and saw
the increment.

## Refs
`NVIM_DAP.md`, `man 1 gdb`, `info gdb` (TUI node), gdb `help layout`, `help stepi`
