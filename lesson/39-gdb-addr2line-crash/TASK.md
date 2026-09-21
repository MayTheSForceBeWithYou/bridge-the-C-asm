# 39 — addr2line on a crash (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. Keep `boom()` as a NULL deref. `make` (`-ggdb`).
2. nvim-dap: optional **F9** on `boom`; launch `./prog`; after crash/stop, note
   the faulting address from **Stacks**.
3. Required: `addr2line -e ./prog -f -C 0xADDR` (use your address).
4. gdb alternative: `run`, `bt`, `info registers rip`, then the same
   `addr2line`. Optional: strip a copy and compare mapping.

## Done when

- You mapped a crash address to `boom` / a source line with `addr2line`.
- Program crashing is expected. You used Stacks and/or `bt`.

## Lookup

`NVIM_DAP.md`, `man 1 addr2line`, `man 1 gdb`, gdb `help bt`, `man 1 strip`.
