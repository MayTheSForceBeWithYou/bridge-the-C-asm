# 10 — Switch: jump table vs compares (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and inspect

1. Implement `dense(int x)` with contiguous cases 0..7.
2. Implement `sparse(int x)` with widely spaced cases (e.g. 0, 10, 100, 1000).
3. `make asm` at `-O0` and `-O2`.
4. Identify table vs compare lowering for each.

## Done when

- You pointed at jump-table evidence (or explained its absence) for `dense`.
- You described how `sparse` decides among cases.
- You noted strategy changes between `-O0` and `-O2`.

## Lookup

`man 1 gcc`, `man 1 objdump`, `man 5 elf`.
