# 36 — gdb TUI loop (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. Implement `sum_to` so `sum_to(10)==55`. `make` → `./prog` prints `ok`.
2. nvim-dap: **F9** on `sum_to`; launch `./prog`; step the loop with **F10** /
   **F11**; watch virtual text / Scopes (optional Watches on the accumulator).
3. Optional gdb TUI: `layout regs`, `layout asm`, `stepi` through several
   iterations; `tui disable` if the UI glitches.

## Done when

- `./prog` prints `ok`.
- You stepped the loop body in nvim-dap and/or TUI and saw the increment live.

## Lookup

`NVIM_DAP.md`, `man 1 gdb`, `info gdb` (TUI), gdb `help layout`, `help stepi`.
