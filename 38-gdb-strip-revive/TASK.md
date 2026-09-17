# 38 — strip then break by address (nvim-dap + gdb) (Track C)

## Goal
Strip a binary, lose symbols, still break using a raw address from
`objdump -d`.

## Tool note
**gdb is primary here.** Breaking by raw address (`break *0xADDR`) is a gdb
strength. You can still **launch** the stripped binary under nvim-dap and try
an address breakpoint via the DAP REPL if CodeLLDB accepts it — treat that as
optional experimentation, not the success criterion.

## Do

1. Implement `secret` to return 42. `make` and confirm `./prog` prints `ok`.
2. Copy and strip:
```
cp prog prog.stripped
strip prog.stripped
nm prog.stripped || true
objdump -d prog.stripped | less
```
3. Find the start address of what was `secret` (same instruction shape as in
   `objdump -d prog` with symbols).

## gdb CLI (primary)

```
gdb -q ./prog.stripped
(gdb) break *0xADDR
(gdb) run
(gdb) disassemble
(gdb) info registers
(gdb) continue
(gdb) quit
```

Replace `0xADDR` with the address you found.

## nvim-dap (optional)

1. `<leader>dc` → Launch existing → `./prog.stripped`.
2. Try DAP REPL (`<leader>dr`) for an LLDB-style breakpoint at an address if
   you know the command; if it fails, that is expected — finish with gdb.
3. With symbols still present on `./prog`, normal F9 breakpoints work fine —
   contrast that with the stripped case.

## Success
You hit a breakpoint in the stripped binary with no symbol names (via gdb).
`strip` / `nm` / `objdump` / `gdb` all used.

## Refs
`NVIM_DAP.md`, `man 1 strip`, `man 1 nm`, `man 1 objdump`, `man 1 gdb`
