# 14 — Tail-call optimization? (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and inspect

1. Implement `fac_naive(long n)` as `n * fac(n-1)` and `fac_tail(long n, long acc)`
   as a tail-recursive helper.
2. Compare `make asm` vs `make O=2 asm`.
3. Confirm with `objdump -d` on both builds.

## Done when

- You said whether `fac_tail` became a self-`jmp` / loop at `-O2`.
- You explained why `fac_naive` did or did not.
- You cited concrete instructions, not vibes.

## Lookup

`man 1 gcc`, `man 1 objdump`, `info gcc`.
