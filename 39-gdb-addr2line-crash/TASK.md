# 39 — addr2line on a crash (nvim-dap + gdb) (Track C)

## Goal
Crash on a NULL deref, capture the faulting address from a backtrace, map it
back with `addr2line`.

## nvim-dap (primary for the crash UI)

1. Keep `boom()` as a NULL deref. Build with `-ggdb` (default): `make`.
2. Open `prog.c`. Optional: **F9** on `boom` so you stop before the deref;
   or launch with no bp and let it crash.
3. `<leader>dc` → Launch existing → `./prog`.
4. After the crash / stop: **Stacks** panel — note the faulting frame / address.
5. Still run `addr2line` (required — stacks alone are not enough for this TASK):
```
addr2line -e ./prog -f -C 0xADDR
```

## gdb CLI (alternative)

```
gdb -q ./prog
(gdb) run
(gdb) bt
(gdb) info registers rip
(gdb) quit
```

Then:
```
addr2line -e ./prog -f -C 0xADDR
```

Compare `addr2line` output to the stacks / `bt` source line. Any difference?
(inlining, missing `-g`, stripped binary — try `strip` a copy and repeat.)

## Success
You mapped a crash address to `boom` / a source line with `addr2line`.
Program crashing is expected. You used Stacks and/or `bt`.

## Refs
`NVIM_DAP.md`, `man 1 addr2line`, `man 1 gdb`, gdb `help bt`, `man 1 strip`
