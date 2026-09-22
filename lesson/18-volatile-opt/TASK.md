# 18 — `volatile` vs optimization (practice)

Read `LESSON.md` first — especially **Real `-O2` contrast** and the navigation
steps. This file is only the lab.

## Build and inspect

1. Implement two busy-looking store/load loops: one with an ordinary `int`, one
   with `volatile int`.
2. `make O=2 asm disasm` and compare (this lesson's point is `-O2`).
3. Answer which loads/stores survive and what happened to the non-volatile loop.

## Done when

- You showed surviving volatile memory operations in the listing.
- You described how the non-volatile loop collapsed or disappeared.
- You gave a one-sentence MMIO-style reason the distinction matters.

## Lookup

`man 1 gcc`, `info gcc`, `man 1 objdump`.
