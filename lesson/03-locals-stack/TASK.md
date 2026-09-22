# 03 — Locals on the stack (practice)

Read `LESSON.md` first — especially **How to read one store line** and the worked
example. This file is only the lab.

## Build and inspect

1. In `work()`, declare at least four locals of mixed types (`int`, `long`, `char`,
   and an array of 4 ints). Touch each so they stay live (addresses, print, or a
   combined return).
2. `make asm`, copy `prog.s` aside, then `make O=2 asm`.
3. Record stack offsets, frame size (or deepest slot), and what `-O2` changes —
   using the Check yourself questions in `LESSON.md`.

## Reading `prog.s`

Search for `work:`. Skip `.cfi_*` and the prologue. List every `-N(%rbp)` store
with its mnemonic width and verbose comment. Map those to your C names.

## Done when

- You mapped each `-O0` local to a `-N(%rbp)` offset (or explained an exception).
- You stated the frame size from `subq` *or* justified an estimate from the
  deepest slot when `subq` is absent.
- You described which locals vanished into registers or disappeared at `-O2`.

## Lookup

`man 1 gcc`, `man 1 objdump`, `info as` (addressing modes).
