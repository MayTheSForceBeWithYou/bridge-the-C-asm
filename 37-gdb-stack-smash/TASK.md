# 37 — gdb stack smash (nvim-dap + gdb) (Track C)

## Goal
Deliberately overflow a small stack buffer and observe the corrupted return
address / crash. **Intentional undefined behavior for learning.**

## nvim-dap (primary)

1. Read `smash()` — it writes past `buf[8]`. Do not "fix" the overflow;
   the point is to watch it.
2. Build with `-fno-stack-protector` (Makefile already sets it): `make`.
3. Open `prog.c`. **F9** inside `smash` (e.g. on the `memset` / first line).
4. `<leader>dc` → Launch existing → `./prog`.
5. On break: `<leader>dm` on `buf` (address from Scopes). Note the pattern
   (`0x41` if you used `memset(..., 0x41, 64)`).
6. **Continue** (`<leader>dc` / F5) and let it crash / return wrongly.
7. Open the **Stacks** panel — compare to gdb `bt`. Frames may look nonsense
   once the return address is smashed; that is the lesson.

## gdb CLI (alternative)

```
make
gdb -q ./prog
(gdb) break smash
(gdb) run
(gdb) stepi
(gdb) x/16xb buf
(gdb) continue
(gdb) bt
(gdb) quit
```

Optionally: `disassemble smash`, note where the saved `%rbp` / return address
sit relative to `buf`.

## Success
You saw the overflow corrupt adjacent stack slots and/or crash at an
unexpected address. Program is *expected* to fail/crash — that is success.

## Warn
Do not copy this pattern into real code. Stack protectors (`-fstack-protector`)
exist for a reason; this exercise turns them off on purpose.

## Refs
`NVIM_DAP.md`, `man 1 gdb`, gdb `help x`, `help bt`, `man 1 gcc`
