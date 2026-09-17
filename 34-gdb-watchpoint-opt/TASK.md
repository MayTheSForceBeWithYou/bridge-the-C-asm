# 34 — gdb watchpoint -O0 vs -O2 (nvim-dap + gdb) (Track C)

## Goal
A data `watch` on a stack local works at `-O0` and silently fails at `-O2` when
the variable is promoted to a register.

## Honest tool note
nvim-dap gives you **conditional breakpoints** (`<leader>dB`) and **watched
expressions** in dap-ui. True **hardware/data watchpoints** (`watch x` in gdb)
are **not reliably exposed** in the CodeLLDB UI — use **gdb** for the `watch`
comparison. Still use nvim-dap to step both `-O0` and `-O2` builds and see
locals vanish into registers.

## nvim-dap (stepping / locals)

1. Build two binaries:
```
make clean && make && cp prog prog.O0
make clean && make O=2 && cp prog prog.O2
```
2. Open `prog.c`. **F9** on `bump`. `<leader>dc` → Launch existing → `./prog.O0`.
3. On stop: Scopes / virtual text for `x` / `i`. Add a Watch on `x`. Step with
   F10 and watch updates at `-O0`.
4. Terminate (`<leader>dt`). Launch `./prog.O2` the same way. Do locals still
   appear? Conditional bp (`<leader>dB`) e.g. stop when `i == 3` if useful.
5. Compare with `make O=0 asm` vs `make O=2 asm` / `disassemble` thinking.

## gdb CLI (data watchpoint — required for the lesson)

Session A (`-O0`):
```
gdb -q ./prog.O0
(gdb) break bump
(gdb) run
(gdb) watch x
(gdb) continue
(gdb) continue
(gdb) quit
```

Session B (`-O2`):
```
gdb -q ./prog.O2
(gdb) break bump
(gdb) run
(gdb) watch x
(gdb) continue
(gdb) quit
```

Did the watchpoint fire at `-O2`? Check `info locals`, `disassemble bump`.

## Success
You observed watch hits at `-O0`, explained `-O2`, and stepped both builds in
nvim-dap and/or gdb. Both binaries print `ok`.

## Refs
`NVIM_DAP.md`, `man 1 gdb`, gdb `help watch`, `man 1 gcc`
