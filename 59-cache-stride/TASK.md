# 59 — cache stride (practice)

Read `LESSON.md` first. This file is only the lab.

## Measure

1. Allocate a large buffer (tens of MiB).
2. Time sequential touch vs stride-64 touch — **same number of accesses**.
3. Explain with the 64-byte L1 line fact. Print `ok`.

## Done when

- Two cycle counts; stride slower; `ok`.

## Lookup

exercise 55, `man 1 gcc`. L1D line = 64 B on this machine.
