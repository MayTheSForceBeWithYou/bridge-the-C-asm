# 50 — -fvisibility=hidden (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and compare

1. `make`; also `gcc -ggdb -O0 -fvisibility=hidden -o prog.hidden prog.c`.
2. Compare `nm` and `readelf --dyn-syms` on both binaries.
3. Note which functions remain exported; reason about executable vs `.so` stakes.

## Done when

- You documented symbol differences before/after `-fvisibility=hidden`.

## Lookup

`man 1 nm`, `man 1 readelf`, `man 1 gcc`, `info gcc`.
