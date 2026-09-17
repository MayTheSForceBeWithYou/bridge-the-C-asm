# 60 — false sharing (Track F)

## Goal
Two pthreads bump adjacent `int`s vs padded-to-64-byte counters; time both.

## Do
1. Implement two threads each incrementing their own counter many times.
2. Layout A: `struct { int a; int b; }` (same cache line).
3. Layout B: pad so each counter owns its own 64-byte line.
4. Link with `-lpthread` (Makefile sets `LDLIBS`).
5. Compare `rdtscp` wall for the pair of threads.

## Success
Padded version typically faster. `./prog` prints both timings and `ok`.

## Refs
`man 3 pthread_create`, exercise 55, `man 1 gcc`
