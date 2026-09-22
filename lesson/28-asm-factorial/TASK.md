# 28 — Recursive factorial in pure asm (practice)

Read `LESSON.md` first — especially the recursion checklist and the "calling
myself does not relax ABI" rule. This file is only the lab.

## Build and run

1. Implement `long fac(long n)` in `fac.s` with real recursive `call`s
   (`n >= 0`; `fac(0) = 1`). Save what you need across the call; return in
   `%rax`.
2. `make && make run`.

## Done when

- Harness prints `ok` for several `n`.
- Disassembly shows `call` to `<fac>` with a save of `n` around it.

## Lookup

`man 1 as`, `info as`; compare to exercises 13–14.
