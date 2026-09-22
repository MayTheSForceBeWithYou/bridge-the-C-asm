# 31 — Conditional move (practice)

Read `LESSON.md` first — especially **Decode a branchless `max`** and the
branchy contrast. This file is only the lab.

## Build and inspect

1. Implement straightforward `int max(int a, int b);` and `int abs_int(int x);`.
2. Compare `make asm` vs `make O=2 asm` (copy aside).

## Done when

- You described the `-O0` shape for at least one function (branchy and/or `cmov`).
- You recorded whether `-O2` emitted `cmov*` or another branchless form.
- You cited instructions from *your* listings.

## Lookup

`man 1 gcc`, `man 1 objdump`, `info as`.
