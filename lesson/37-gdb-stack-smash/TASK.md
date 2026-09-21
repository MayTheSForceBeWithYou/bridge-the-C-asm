# 37 — gdb stack smash (practice)

Read `LESSON.md` first. This file is only the lab.

**Intentional undefined behavior for learning.** Do not "fix" the overflow.

## Do

1. Read `smash()` — it writes past `buf[8]`. `make` (already `-fno-stack-protector`).
2. nvim-dap: **F9** inside `smash`; launch `./prog`; **`<leader>dm`** on `buf`;
   note `0x41` pattern; continue and inspect **Stacks** after the mess.
3. gdb alternative: `break smash`, `run`, `x/16xb buf`, `continue`, `bt`;
   optional `disassemble smash`.

## Done when

- You saw the overflow corrupt adjacent stack slots and/or crash at an
  unexpected address. Failure/crash of `./prog` is expected success.

## Warn

Do not copy this into real code. Stack protectors exist for a reason.

## Lookup

`NVIM_DAP.md`, `man 1 gdb`, gdb `help x`, `help bt`, `man 1 gcc`.
