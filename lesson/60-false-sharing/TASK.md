# 60 — false sharing (practice)

Read `LESSON.md` first. This file is only the lab.

## Implement

1. Two threads each increment their own counter many times.
2. Layout A: adjacent ints; Layout B: pad to 64-byte line ownership each.
3. Link `-lpthread`; compare `rdtscp` timings; print both and `ok`.

## Done when

- Padded version typically faster; both timings printed; `ok`.

## Lookup

`man 3 pthread_create`, exercise 55, `man 1 gcc`.
