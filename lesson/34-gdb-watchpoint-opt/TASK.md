# 34 — gdb watchpoint -O0 vs -O2 (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. Build both binaries:
   ```
   make clean && make && cp prog prog.O0
   make clean && make O=2 && cp prog prog.O2
   ```
2. nvim-dap: **F9** on `bump`; launch `./prog.O0`, Watch `x`, step with F10.
   Terminate; launch `./prog.O2` and compare locals / Scopes.
3. gdb (required for data watch): on `./prog.O0` — `break bump`, `run`,
   `watch x`, `continue` until you see hits. On `./prog.O2` — same; note whether
   the watch fires. Check `info locals`, `disassemble bump`.
4. Optional: `make O=0 asm` vs `make O=2 asm`.

## Done when

- Both binaries print `ok`.
- You saw watch hits at `-O0`, explained the `-O2` outcome, and stepped both
  builds in nvim-dap and/or gdb.

## Lookup

`NVIM_DAP.md`, `man 1 gdb`, gdb `help watch`, `man 1 gcc`.
