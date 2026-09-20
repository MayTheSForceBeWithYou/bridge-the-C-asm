# 61 — optional perf stat (practice)

Read `LESSON.md` first. This file is only the lab. Optional.

## Do

1. `command -v perf || echo 'optional: sudo pacman -S perf'`
2. If present: `perf stat -e cycles,instructions,branches,branch-misses ./prog`
   (ideally ex 58 sorted vs shuffled).
3. If missing: implement/time a simple loop with `rdtscp`; note "perf skipped".

## Done when

- Perf counter table recorded, **or** explicit skip note + rdtscp fallback.

## Lookup

exercises 55/58. No perf manual in REFS.md.
