# 62 — beat -O2 capstone (Track F)

## Goal
Naive scalar loop is given (branch-heavy / unfriendly). Restructure your
version to beat the compiler's own `-O2` of the **naive** code on your
`rdtscp` measurement.

## Do
1. Read `naive_sum` — do not "optimize the compiler"; measure it at `-O2`.
2. Write `fast_sum` with a better approach (branchless, blocked, etc.).
3. Protocol:
```
make O=2
./prog
```
Harness times both; fails if `fast_sum` is not faster than `naive_sum` on
the same input (allow a small warmup). Re-run a few times.

## Success
`./prog` prints both cycle counts and `ok` when fast wins.

## Refs
exercises 55–60, 31; `man 1 gcc`
