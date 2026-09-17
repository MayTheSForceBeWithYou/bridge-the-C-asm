# 35 — gdb stack after call (nvim-dap + gdb) (Track C)

## Goal
See the return address on the stack right after `call`, and watch `%rsp`
move across pushes.

## nvim-dap (primary for call / memory; gdb for stepi precision)

1. Implement `scratch` in `scratch.s`: push three regs, pop them, return 42.
2. `make` → `./stack_test`.
3. Open `harness.c` (or wherever `call_site` / the call lives). **F9** at the
   call site into `scratch`.
4. `<leader>dc` → Launch existing → `./stack_test`.
5. After stopping at the call site, **F11** into `scratch`. Note SP / `$rsp`
   in Scopes (registers).
6. `<leader>dm` — memory view at the stack pointer address. You should see the
   return address slot and later pushed values as you step.
7. **Honest limit:** F10/F11 are usually **source/line** steps, not `stepi`.
   To watch **each** `push` drop `%rsp` by 8, use the gdb session below.

## gdb CLI (alternative / instruction-precise)

```
gdb -q ./stack_test
(gdb) break call_site
(gdb) run
(gdb) stepi
(gdb) x/8xg $rsp
(gdb) break scratch
(gdb) continue
(gdb) info registers rsp
(gdb) stepi
(gdb) stepi
(gdb) stepi
(gdb) info registers rsp
(gdb) x/8xg $rsp
(gdb) continue
(gdb) quit
```

After `call` enters `scratch`, how many bytes does `%rsp` drop per push?

## Success
`./stack_test` prints `ok`. You can explain the 8-byte slots at `$rsp`
(nvim-dap memory and/or `x/8xg`).

## Refs
`NVIM_DAP.md`, `man 1 gdb`, gdb `help x`, `man 1 as`
