# 03 — Locals on the stack (Track A)

## Goal
Find where several local variables live at `-O0` vs `-O2`.

## Do
1. In `work()`, declare at least four locals of mixed types (`int`, `long`,
   `char`, maybe an array of 4 ints). Touch each so they are not DCE'd
   (e.g. take addresses, print, or return a combination).
2. Emit asm at `-O0` and `-O2`.

## Observe / answer
- At `-O0`, what stack offsets (`-N(%rbp)`) hold each local?
- At `-O2`, which locals vanish into registers or disappear entirely?
- How large is the stack frame (`sub $N,%rsp` or equivalent)?

## Refs
`man 1 gcc`, `man 1 objdump`, `info as` (addressing modes)

Also in nvim-dap: after `make`, launch `./prog` via Launch existing — see `NVIM_DAP.md`.