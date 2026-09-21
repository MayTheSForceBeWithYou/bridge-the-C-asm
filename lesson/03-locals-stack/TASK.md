# 03 — Locals on the stack (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and inspect

1. In `work()`, declare at least four locals of mixed types (`int`, `long`, `char`,
   and an array of 4 ints). Touch each so they stay live (addresses, print, or a
   combined return).
2. Emit asm at `-O0` and `-O2`.
3. Record stack offsets, frame size, and what `-O2` changes — using the questions
   in `LESSON.md`.

## Done when

- You mapped each `-O0` local to a `-N(%rbp)` offset (or explained an exception).
- You stated the frame size from the listing.
- You described which locals vanished into registers or disappeared at `-O2`.

## Lookup

`man 1 gcc`, `man 1 objdump`, `info as` (addressing modes).
