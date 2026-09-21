# 09 — Signed vs unsigned compares (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and inspect

1. Implement `cmp_signed` and `cmp_unsigned` returning a negative / zero /
   positive style result.
2. Write a loop that exits when an index hits a limit — once `int`, once
   `unsigned`. Prefer limits near the top of the range conceptually.
3. `make asm`. Record jump mnemonics for each.

## Done when

- You listed signed vs unsigned conditional-jump mnemonics from *your* `.s`.
- You can say (in one sentence) what flag story those jumps consult.
- You tied a loop-exit jump back to the C type that caused it.

## Lookup

`man 1 objdump`, `info as`, `man 1 gcc`.
