# 61 — optional perf stat (Track F)

## Goal
**Optional.** If `perf` is installed, cross-check an earlier timing with
hardware counters. Otherwise skip with the pacman hint.

## Do
1. Check: `command -v perf || echo 'optional: sudo pacman -S perf'`
2. If present, time exercise 58's binary (or this dir's `prog`) with:
```
perf stat -e cycles,instructions,branches,branch-misses ./prog
```
3. Compare branch-misses sorted vs shuffled if you rebuild those modes.
4. If `perf` missing: still implement `prog` (simple loop) and rely on
   `rdtscp` from 55 — note "perf skipped" in your notes.

## Success
Either perf counter table recorded, or explicit skip note + rdtscp fallback.

## Refs
exercise 55/58 — no perf manual in REFS.md. Use `perf help stat` if the
tool is present; otherwise rdtscp fallback only.
