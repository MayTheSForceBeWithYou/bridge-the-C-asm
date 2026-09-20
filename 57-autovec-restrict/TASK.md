# 57 — autovec vs aliasing / restrict (practice)

Read `LESSON.md` first. This file is only the lab.

## Implement and inspect

1. `add_arrays`, `add_hazard`, and `add_restrict` (`restrict` pointers).
2. Build with `-O3 -march=native -fopt-info-vec-optimized` (stderr → `vec.txt`).
3. Confirm vectorization reports; look for packed ops in `objdump -d`.

## Done when

- `vec.txt` shows safe/`restrict` vectorized; hazard not until fixed; `./prog` prints `ok`.

## Lookup

`man 1 gcc`, `man 1 objdump`.
