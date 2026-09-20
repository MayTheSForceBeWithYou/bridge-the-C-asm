# 55 — rdtscp cycle timer (practice)

Read `LESSON.md` first. This file is only the lab.

## Implement

1. `rdtscp_now()` via Extended Asm or tiny `.s` — **`rdtscp`**, not `rdtsc`.
2. Time ~10M increments; print elapsed cycles.
3. Run thrice; note variance. Print `ok`.

## Done when

- `./prog` prints three positive cycle deltas and `ok`.

## Lookup

`man 1 gcc`, `info gcc`, exercise 19. This CPU: `rdtscp` + `constant_tsc`.
