# 57 — autovec vs aliasing / restrict (Track F)

## Goal
See `-O3` vectorize one loop and refuse another until `restrict` fixes the
aliasing hazard. Use `-fopt-info-vec-optimized` (works on GCC 16.2.1 here).

## Do
1. Implement `add_arrays` (clear, no alias) and `add_hazard` (params that
   could alias). Then `add_restrict` with `restrict` pointers.
2. Build:
```
make clean
make CFLAGS='-ggdb -Wall -Wextra -O3 -march=native -fopt-info-vec-optimized' 2> vec.txt
cat vec.txt
objdump -d prog | less
```
3. Look for packed ops (`paddd`/`vaddps`/etc.) on the vectorized path.

## Success
`vec.txt` reports vectorization for the safe/`restrict` loops; hazard does
not vectorize until fixed. `./prog` prints `ok`.

## Refs
`man 1 gcc`, `man 1 objdump`
