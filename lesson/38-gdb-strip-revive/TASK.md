# 38 — strip then break by address (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. Implement `secret` → 42. `make`; confirm `./prog` prints `ok`.
2. `cp prog prog.stripped && strip prog.stripped`. Run `nm prog.stripped || true`
   and `objdump -d prog.stripped`. Find the address of what was `secret` by
   matching instruction shape to `objdump -d prog`.
3. gdb (primary): `break *0xADDR`, `run`, `disassemble`, `continue`.
4. Optional: launch `./prog.stripped` under nvim-dap and experiment with an
   address break in the DAP REPL — not required if it fails.

## Done when

- You hit a breakpoint in the stripped binary with no symbol names (via gdb).
- You used `strip` / `nm` / `objdump` / `gdb`.

## Lookup

`NVIM_DAP.md`, `man 1 strip`, `man 1 nm`, `man 1 objdump`, `man 1 gdb`.
